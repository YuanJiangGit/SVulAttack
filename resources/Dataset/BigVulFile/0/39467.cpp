static int yam_info_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &yam_seqops);
}
