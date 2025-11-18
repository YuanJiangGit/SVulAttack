static int khazad_setkey(struct crypto_tfm *tfm, const u8 *in_key,
			 unsigned int key_len)
{
	struct khazad_ctx *ctx = crypto_tfm_ctx(tfm);
	const __be32 *key = (const __be32 *)in_key;
	int r;
	const u64 *S = T7;
	u64 K2, K1;

	/* key is supposed to be 32-bit aligned */
	K2 = ((u64)be32_to_cpu(key[0]) << 32) | be32_to_cpu(key[1]);
	K1 = ((u64)be32_to_cpu(key[2]) << 32) | be32_to_cpu(key[3]);

	/* setup the encrypt key */
	for (r = 0; r <= KHAZAD_ROUNDS; r++) {
		ctx->E[r] = T0[(int)(K1 >> 56)       ] ^
			    T1[(int)(K1 >> 48) & 0xff] ^
			    T2[(int)(K1 >> 40) & 0xff] ^
			    T3[(int)(K1 >> 32) & 0xff] ^
			    T4[(int)(K1 >> 24) & 0xff] ^
			    T5[(int)(K1 >> 16) & 0xff] ^
			    T6[(int)(K1 >>  8) & 0xff] ^
			    T7[(int)(K1      ) & 0xff] ^
			    c[r] ^ K2;
		K2 = K1; 
		K1 = ctx->E[r];
	}
	/* Setup the decrypt key */
	ctx->D[0] = ctx->E[KHAZAD_ROUNDS];
	for (r = 1; r < KHAZAD_ROUNDS; r++) {
		K1 = ctx->E[KHAZAD_ROUNDS - r];
		ctx->D[r] = T0[(int)S[(int)(K1 >> 56)       ] & 0xff] ^
			    T1[(int)S[(int)(K1 >> 48) & 0xff] & 0xff] ^
			    T2[(int)S[(int)(K1 >> 40) & 0xff] & 0xff] ^
			    T3[(int)S[(int)(K1 >> 32) & 0xff] & 0xff] ^
			    T4[(int)S[(int)(K1 >> 24) & 0xff] & 0xff] ^
			    T5[(int)S[(int)(K1 >> 16) & 0xff] & 0xff] ^
			    T6[(int)S[(int)(K1 >>  8) & 0xff] & 0xff] ^
			    T7[(int)S[(int)(K1      ) & 0xff] & 0xff];
	}
	ctx->D[KHAZAD_ROUNDS] = ctx->E[0];

	return 0;

}
