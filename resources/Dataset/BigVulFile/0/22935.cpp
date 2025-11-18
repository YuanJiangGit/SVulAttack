static void nfs4_free_lock_state(struct nfs4_lock_state *lsp)
{
	struct nfs_client *clp = lsp->ls_state->owner->so_client;

	spin_lock(&clp->cl_lock);
	nfs_free_unique_id(&clp->cl_lockowner_id, &lsp->ls_id);
	spin_unlock(&clp->cl_lock);
	rpc_destroy_wait_queue(&lsp->ls_sequence.wait);
	kfree(lsp);
}
