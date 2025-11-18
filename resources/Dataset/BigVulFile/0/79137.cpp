static ssize_t ucma_join_multicast(struct ucma_file *file,
				   const char __user *inbuf,
				   int in_len, int out_len)
{
	struct rdma_ucm_join_mcast cmd;

	if (copy_from_user(&cmd, inbuf, sizeof(cmd)))
		return -EFAULT;

	if (!rdma_addr_size_kss(&cmd.addr))
		return -EINVAL;

	return ucma_process_join(file, &cmd, out_len);
}
