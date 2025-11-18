cleanup_bmc_device(struct kref *ref)
{
	struct bmc_device *bmc = container_of(ref, struct bmc_device, usecount);

	/*
	 * Remove the platform device in a work queue to avoid issues
	 * with removing the device attributes while reading a device
	 * attribute.
	 */
	schedule_work(&bmc->remove_work);
}
