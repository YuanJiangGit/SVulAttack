static int ptrace_peek_siginfo(struct task_struct *child,
				unsigned long addr,
				unsigned long data)
{
	struct ptrace_peeksiginfo_args arg;
	struct sigpending *pending;
	struct sigqueue *q;
	int ret, i;

	ret = copy_from_user(&arg, (void __user *) addr,
				sizeof(struct ptrace_peeksiginfo_args));
	if (ret)
		return -EFAULT;

	if (arg.flags & ~PTRACE_PEEKSIGINFO_SHARED)
		return -EINVAL; /* unknown flags */

	if (arg.nr < 0)
		return -EINVAL;

	/* Ensure arg.off fits in an unsigned long */
	if (arg.off > ULONG_MAX)
		return 0;

	if (arg.flags & PTRACE_PEEKSIGINFO_SHARED)
		pending = &child->signal->shared_pending;
	else
		pending = &child->pending;

	for (i = 0; i < arg.nr; ) {
		kernel_siginfo_t info;
		unsigned long off = arg.off + i;
		bool found = false;

		spin_lock_irq(&child->sighand->siglock);
		list_for_each_entry(q, &pending->list, list) {
			if (!off--) {
				found = true;
				copy_siginfo(&info, &q->info);
				break;
			}
		}
		spin_unlock_irq(&child->sighand->siglock);

		if (!found) /* beyond the end of the list */
			break;

#ifdef CONFIG_COMPAT
		if (unlikely(in_compat_syscall())) {
			compat_siginfo_t __user *uinfo = compat_ptr(data);

			if (copy_siginfo_to_user32(uinfo, &info)) {
				ret = -EFAULT;
				break;
			}

		} else
#endif
		{
			siginfo_t __user *uinfo = (siginfo_t __user *) data;

			if (copy_siginfo_to_user(uinfo, &info)) {
				ret = -EFAULT;
				break;
			}
		}

		data += sizeof(siginfo_t);
		i++;

		if (signal_pending(current))
			break;

		cond_resched();
	}

	if (i > 0)
		return i;

	return ret;
}
