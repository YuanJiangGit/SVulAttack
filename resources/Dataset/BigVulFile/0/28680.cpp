static ssize_t lbs_failcount_read(struct file *file, char __user *userbuf,
				  size_t count, loff_t *ppos)
{
	return lbs_threshold_read(TLV_TYPE_FAILCOUNT, CMD_SUBSCRIBE_FAILCOUNT,
				  file, userbuf, count, ppos);
}
