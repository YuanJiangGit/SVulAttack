static int part_get_info_by_dev_and_name(const char *dev_iface,
					 const char *dev_part_str,
					 struct blk_desc **dev_desc,
					 disk_partition_t *part_info)
{
	char *ep;
	const char *part_str;
	int dev_num;

	part_str = strchr(dev_part_str, '#');
	if (!part_str || part_str == dev_part_str)
		return -EINVAL;

	dev_num = simple_strtoul(dev_part_str, &ep, 16);
	if (ep != part_str) {
		/* Not all the first part before the # was parsed. */
		return -EINVAL;
	}
	part_str++;

	*dev_desc = blk_get_dev(dev_iface, dev_num);
	if (!*dev_desc) {
		printf("Could not find %s %d\n", dev_iface, dev_num);
		return -EINVAL;
	}
	if (part_get_info_by_name(*dev_desc, part_str, part_info) < 0) {
		printf("Could not find \"%s\" partition\n", part_str);
		return -EINVAL;
	}
	return 0;
}
