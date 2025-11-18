no_context(struct pt_regs *regs, unsigned long error_code,
	   unsigned long address)
{
	struct task_struct *tsk = current;
	unsigned long *stackend;
	unsigned long flags;
	int sig;

	/* Are we prepared to handle this kernel fault? */
	if (fixup_exception(regs))
		return;

	/*
	 * 32-bit:
	 *
	 *   Valid to do another page fault here, because if this fault
	 *   had been triggered by is_prefetch fixup_exception would have
	 *   handled it.
	 *
	 * 64-bit:
	 *
	 *   Hall of shame of CPU/BIOS bugs.
	 */
	if (is_prefetch(regs, error_code, address))
		return;

	if (is_errata93(regs, address))
		return;

	/*
	 * Oops. The kernel tried to access some bad page. We'll have to
	 * terminate things with extreme prejudice:
	 */
	flags = oops_begin();

	show_fault_oops(regs, error_code, address);

	stackend = end_of_stack(tsk);
	if (tsk != &init_task && *stackend != STACK_END_MAGIC)
		printk(KERN_ALERT "Thread overran stack, or stack corrupted\n");

	tsk->thread.cr2		= address;
	tsk->thread.trap_no	= 14;
	tsk->thread.error_code	= error_code;

	sig = SIGKILL;
	if (__die("Oops", regs, error_code))
		sig = 0;

	/* Executive summary in case the body of the oops scrolled away */
	printk(KERN_EMERG "CR2: %016lx\n", address);

	oops_end(flags, regs, sig);
}
