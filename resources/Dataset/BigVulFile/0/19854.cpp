static int nfs41_lock_expired(struct nfs4_state *state, struct file_lock *request)
{
	int status = NFS_OK;

	if (test_bit(LK_STATE_IN_USE, &state->flags))
		status = nfs41_check_expired_locks(state);
	if (status == NFS_OK)
		return status;
	return nfs4_lock_expired(state, request);
}
