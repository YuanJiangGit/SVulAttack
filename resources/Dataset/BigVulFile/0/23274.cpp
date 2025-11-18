static int decode_attr_mode(struct xdr_stream *xdr, uint32_t *bitmap, umode_t *mode)
{
	uint32_t tmp;
	__be32 *p;
	int ret = 0;

	*mode = 0;
	if (unlikely(bitmap[1] & (FATTR4_WORD1_MODE - 1U)))
		return -EIO;
	if (likely(bitmap[1] & FATTR4_WORD1_MODE)) {
		p = xdr_inline_decode(xdr, 4);
		if (unlikely(!p))
			goto out_overflow;
		tmp = be32_to_cpup(p);
		*mode = tmp & ~S_IFMT;
		bitmap[1] &= ~FATTR4_WORD1_MODE;
		ret = NFS_ATTR_FATTR_MODE;
	}
	dprintk("%s: file mode=0%o\n", __func__, (unsigned int)*mode);
	return ret;
out_overflow:
	print_overflow_msg(__func__, xdr);
	return -EIO;
}
