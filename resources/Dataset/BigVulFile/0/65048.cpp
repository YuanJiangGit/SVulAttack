static void check_reg_overflow(struct bpf_reg_state *reg)
{
	if (reg->max_value > BPF_REGISTER_MAX_RANGE)
		reg->max_value = BPF_REGISTER_MAX_RANGE;
	if (reg->min_value < BPF_REGISTER_MIN_RANGE ||
	    reg->min_value > BPF_REGISTER_MAX_RANGE)
		reg->min_value = BPF_REGISTER_MIN_RANGE;
}
