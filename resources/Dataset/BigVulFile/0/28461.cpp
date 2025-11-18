static ssize_t aac_show_max_id(struct device *device,
			       struct device_attribute *attr, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%d\n",
	  class_to_shost(device)->max_id);
}
