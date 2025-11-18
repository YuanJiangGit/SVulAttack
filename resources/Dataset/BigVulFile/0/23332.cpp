static int decode_sessionid(struct xdr_stream *xdr, struct nfs4_sessionid *sid)
{
	return decode_opaque_fixed(xdr, sid->data, NFS4_MAX_SESSIONID_LEN);
}
