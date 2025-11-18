static void bm_evict_inode(struct inode *inode)
{
	clear_inode(inode);
	kfree(inode->i_private);
}
