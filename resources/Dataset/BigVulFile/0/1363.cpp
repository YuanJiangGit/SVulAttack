static __be32 *xdr_inline_decode(struct xdr_stream *xdr, size_t nbytes)
{
	__be32 *p;

	if (nbytes == 0)
		return xdr->p;
	if (xdr->p == xdr->end)
		return NULL;
	p = __xdr_inline_decode(xdr, nbytes);

	return p;
}
