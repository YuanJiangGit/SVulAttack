static bool is_ctx_reg(struct bpf_verifier_env *env, int regno)
{
	const struct bpf_reg_state *reg = cur_regs(env) + regno;

	return reg->type == PTR_TO_CTX;
}
