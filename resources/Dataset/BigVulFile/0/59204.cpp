static int shash_update_unaligned(struct shash_desc *desc, const u8 *data,
				  unsigned int len)
{
	struct crypto_shash *tfm = desc->tfm;
	struct shash_alg *shash = crypto_shash_alg(tfm);
	unsigned long alignmask = crypto_shash_alignmask(tfm);
	unsigned int unaligned_len = alignmask + 1 -
				     ((unsigned long)data & alignmask);
	u8 ubuf[shash_align_buffer_size(unaligned_len, alignmask)]
		__aligned_largest;
	u8 *buf = PTR_ALIGN(&ubuf[0], alignmask + 1);
	int err;

	if (unaligned_len > len)
		unaligned_len = len;

	memcpy(buf, data, unaligned_len);
	err = shash->update(desc, buf, unaligned_len);
	memset(buf, 0, unaligned_len);

	return err ?:
	       shash->update(desc, data + unaligned_len, len - unaligned_len);
}
