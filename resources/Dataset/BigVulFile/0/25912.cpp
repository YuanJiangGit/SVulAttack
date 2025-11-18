static int putreg32(struct task_struct *child, unsigned regno, u32 value)
{
	struct pt_regs *regs = task_pt_regs(child);

	switch (regno) {

	SEG32(cs);
	SEG32(ds);
	SEG32(es);
	SEG32(fs);
	SEG32(gs);
	SEG32(ss);

	R32(ebx, bx);
	R32(ecx, cx);
	R32(edx, dx);
	R32(edi, di);
	R32(esi, si);
	R32(ebp, bp);
	R32(eax, ax);
	R32(eip, ip);
	R32(esp, sp);

	case offsetof(struct user32, regs.orig_eax):
		/*
		 * A 32-bit debugger setting orig_eax means to restore
		 * the state of the task restarting a 32-bit syscall.
		 * Make sure we interpret the -ERESTART* codes correctly
		 * in case the task is not actually still sitting at the
		 * exit from a 32-bit syscall with TS_COMPAT still set.
		 */
		regs->orig_ax = value;
		if (syscall_get_nr(child, regs) >= 0)
			task_thread_info(child)->status |= TS_COMPAT;
		break;

	case offsetof(struct user32, regs.eflags):
		return set_flags(child, value);

	case offsetof(struct user32, u_debugreg[0]) ...
		offsetof(struct user32, u_debugreg[7]):
		regno -= offsetof(struct user32, u_debugreg[0]);
		return ptrace_set_debugreg(child, regno / 4, value);

	default:
		if (regno > sizeof(struct user32) || (regno & 3))
			return -EIO;

		/*
		 * Other dummy fields in the virtual user structure
		 * are ignored
		 */
		break;
	}
	return 0;
}
