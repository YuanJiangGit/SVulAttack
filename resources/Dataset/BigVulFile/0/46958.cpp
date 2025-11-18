static int ctr_crypt(struct blkcipher_desc *desc, struct scatterlist *dst,
		     struct scatterlist *src, unsigned int nbytes)
{
	struct blkcipher_walk walk;
	int err;

	blkcipher_walk_init(&walk, dst, src, nbytes);
	err = blkcipher_walk_virt_block(desc, &walk, DES3_EDE_BLOCK_SIZE);

	while ((nbytes = walk.nbytes) >= DES3_EDE_BLOCK_SIZE) {
		nbytes = __ctr_crypt(desc, &walk);
		err = blkcipher_walk_done(desc, &walk, nbytes);
	}

	if (walk.nbytes) {
		ctr_crypt_final(crypto_blkcipher_ctx(desc->tfm), &walk);
		err = blkcipher_walk_done(desc, &walk, 0);
	}

	return err;
}
