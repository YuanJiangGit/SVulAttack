void fpu__init_prepare_fx_sw_frame(void)
{
	int size = fpu_user_xstate_size + FP_XSTATE_MAGIC2_SIZE;

	fx_sw_reserved.magic1 = FP_XSTATE_MAGIC1;
	fx_sw_reserved.extended_size = size;
	fx_sw_reserved.xfeatures = xfeatures_mask;
	fx_sw_reserved.xstate_size = fpu_user_xstate_size;

	if (IS_ENABLED(CONFIG_IA32_EMULATION) ||
	    IS_ENABLED(CONFIG_X86_32)) {
		int fsave_header_size = sizeof(struct fregs_state);

		fx_sw_reserved_ia32 = fx_sw_reserved;
		fx_sw_reserved_ia32.extended_size = size + fsave_header_size;
	}
}
