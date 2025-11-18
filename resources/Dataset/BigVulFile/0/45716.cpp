static struct crypto_instance *eseqiv_alloc(struct rtattr **tb)
{
	struct crypto_instance *inst;
	int err;

	err = crypto_get_default_rng();
	if (err)
		return ERR_PTR(err);

	inst = skcipher_geniv_alloc(&eseqiv_tmpl, tb, 0, 0);
	if (IS_ERR(inst))
		goto put_rng;

	err = -EINVAL;
	if (inst->alg.cra_ablkcipher.ivsize != inst->alg.cra_blocksize)
		goto free_inst;

	inst->alg.cra_ablkcipher.givencrypt = eseqiv_givencrypt_first;

	inst->alg.cra_init = eseqiv_init;
	inst->alg.cra_exit = skcipher_geniv_exit;

	inst->alg.cra_ctxsize = sizeof(struct eseqiv_ctx);
	inst->alg.cra_ctxsize += inst->alg.cra_ablkcipher.ivsize;

out:
	return inst;

free_inst:
	skcipher_geniv_free(inst);
	inst = ERR_PTR(err);
put_rng:
	crypto_put_default_rng();
	goto out;
}
