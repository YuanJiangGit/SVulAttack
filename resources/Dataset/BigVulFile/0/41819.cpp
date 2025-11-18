static int if6_seq_open(struct inode *inode, struct file *file)
{
	return seq_open_net(inode, file, &if6_seq_ops,
			    sizeof(struct if6_iter_state));
}
