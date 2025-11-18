static unsigned int __user *signal_frame_32_regs(unsigned int sp,
				unsigned int next_sp, unsigned int next_ip)
{
	struct mcontext32 __user *mctx = NULL;
	struct signal_frame_32 __user *sf;
	struct rt_signal_frame_32 __user *rt_sf;

	/*
	 * Note: the next_sp - sp >= signal frame size check
	 * is true when next_sp < sp, for example, when
	 * transitioning from an alternate signal stack to the
	 * normal stack.
	 */
	if (next_sp - sp >= sizeof(struct signal_frame_32) &&
	    is_sigreturn_32_address(next_ip, sp) &&
	    sane_signal_32_frame(sp)) {
		sf = (struct signal_frame_32 __user *) (unsigned long) sp;
		mctx = &sf->mctx;
	}

	if (!mctx && next_sp - sp >= sizeof(struct rt_signal_frame_32) &&
	    is_rt_sigreturn_32_address(next_ip, sp) &&
	    sane_rt_signal_32_frame(sp)) {
		rt_sf = (struct rt_signal_frame_32 __user *) (unsigned long) sp;
		mctx = &rt_sf->uc.uc_mcontext;
	}

	if (!mctx)
		return NULL;
	return mctx->mc_gregs;
}
