SMB2_flush(const unsigned int xid, struct cifs_tcon *tcon, u64 persistent_fid,
	   u64 volatile_fid)
{
	struct smb2_flush_req *req;
	struct TCP_Server_Info *server;
	struct cifs_ses *ses = tcon->ses;
	struct kvec iov[1];
	int resp_buftype;
	int rc = 0;

	cifs_dbg(FYI, "Flush\n");

	if (ses && (ses->server))
		server = ses->server;
	else
		return -EIO;

	rc = small_smb2_init(SMB2_FLUSH, tcon, (void **) &req);
	if (rc)
		return rc;

	req->PersistentFileId = persistent_fid;
	req->VolatileFileId = volatile_fid;

	iov[0].iov_base = (char *)req;
	/* 4 for rfc1002 length field */
	iov[0].iov_len = get_rfc1002_length(req) + 4;

	rc = SendReceive2(xid, ses, iov, 1, &resp_buftype, 0);

	if ((rc != 0) && tcon)
		cifs_stats_fail_inc(tcon, SMB2_FLUSH_HE);

	free_rsp_buf(resp_buftype, iov[0].iov_base);
	return rc;
}
