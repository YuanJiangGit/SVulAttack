static void reg_set_min_max(struct bpf_reg_state *true_reg,
			    struct bpf_reg_state *false_reg, u64 val,
			    u8 opcode)
{
	switch (opcode) {
	case BPF_JEQ:
		/* If this is false then we know nothing Jon Snow, but if it is
		 * true then we know for sure.
		 */
		true_reg->max_value = true_reg->min_value = val;
		break;
	case BPF_JNE:
		/* If this is true we know nothing Jon Snow, but if it is false
		 * we know the value for sure;
		 */
		false_reg->max_value = false_reg->min_value = val;
		break;
	case BPF_JGT:
		/* Unsigned comparison, the minimum value is 0. */
		false_reg->min_value = 0;
		/* fallthrough */
	case BPF_JSGT:
		/* If this is false then we know the maximum val is val,
		 * otherwise we know the min val is val+1.
		 */
		false_reg->max_value = val;
		true_reg->min_value = val + 1;
		break;
	case BPF_JGE:
		/* Unsigned comparison, the minimum value is 0. */
		false_reg->min_value = 0;
		/* fallthrough */
	case BPF_JSGE:
		/* If this is false then we know the maximum value is val - 1,
		 * otherwise we know the mimimum value is val.
		 */
		false_reg->max_value = val - 1;
		true_reg->min_value = val;
		break;
	default:
		break;
	}

	check_reg_overflow(false_reg);
	check_reg_overflow(true_reg);
}
