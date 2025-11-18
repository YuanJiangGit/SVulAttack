void __do_SAK(struct tty_struct *tty)
{
#ifdef TTY_SOFT_SAK
	tty_hangup(tty);
#else
	struct task_struct *g, *p;
	struct pid *session;
	int		i;

	if (!tty)
		return;
	session = tty->session;

	tty_ldisc_flush(tty);

	tty_driver_flush_buffer(tty);

	read_lock(&tasklist_lock);
	/* Kill the entire session */
	do_each_pid_task(session, PIDTYPE_SID, p) {
		tty_notice(tty, "SAK: killed process %d (%s): by session\n",
			   task_pid_nr(p), p->comm);
		send_sig(SIGKILL, p, 1);
	} while_each_pid_task(session, PIDTYPE_SID, p);

	/* Now kill any processes that happen to have the tty open */
	do_each_thread(g, p) {
		if (p->signal->tty == tty) {
			tty_notice(tty, "SAK: killed process %d (%s): by controlling tty\n",
				   task_pid_nr(p), p->comm);
			send_sig(SIGKILL, p, 1);
			continue;
		}
		task_lock(p);
		i = iterate_fd(p->files, 0, this_tty, tty);
		if (i != 0) {
			tty_notice(tty, "SAK: killed process %d (%s): by fd#%d\n",
				   task_pid_nr(p), p->comm, i - 1);
			force_sig(SIGKILL, p);
		}
		task_unlock(p);
	} while_each_thread(g, p);
	read_unlock(&tasklist_lock);
#endif
}
