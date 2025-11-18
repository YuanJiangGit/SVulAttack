static void tty_ldisc_restore(struct tty_struct *tty, struct tty_ldisc *old)
{
	struct tty_ldisc *new_ldisc;
	int r;

	/* There is an outstanding reference here so this is safe */
	old = tty_ldisc_get(tty, old->ops->num);
	WARN_ON(IS_ERR(old));
	tty->ldisc = old;
	tty_set_termios_ldisc(tty, old->ops->num);
	if (tty_ldisc_open(tty, old) < 0) {
		tty_ldisc_put(old);
		/* This driver is always present */
		new_ldisc = tty_ldisc_get(tty, N_TTY);
		if (IS_ERR(new_ldisc))
			panic("n_tty: get");
		tty->ldisc = new_ldisc;
		tty_set_termios_ldisc(tty, N_TTY);
		r = tty_ldisc_open(tty, new_ldisc);
		if (r < 0)
			panic("Couldn't open N_TTY ldisc for "
			      "%s --- error %d.",
			      tty_name(tty), r);
	}
}
