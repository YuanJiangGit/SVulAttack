static struct block_device *bd_start_claiming(struct block_device *bdev,
					      void *holder)
{
	struct gendisk *disk;
	struct block_device *whole;
	int partno, err;

	might_sleep();

	/*
	 * @bdev might not have been initialized properly yet, look up
	 * and grab the outer block device the hard way.
	 */
	disk = get_gendisk(bdev->bd_dev, &partno);
	if (!disk)
		return ERR_PTR(-ENXIO);

	/*
	 * Normally, @bdev should equal what's returned from bdget_disk()
	 * if partno is 0; however, some drivers (floppy) use multiple
	 * bdev's for the same physical device and @bdev may be one of the
	 * aliases.  Keep @bdev if partno is 0.  This means claimer
	 * tracking is broken for those devices but it has always been that
	 * way.
	 */
	if (partno)
		whole = bdget_disk(disk, 0);
	else
		whole = bdgrab(bdev);

	module_put(disk->fops->owner);
	put_disk(disk);
	if (!whole)
		return ERR_PTR(-ENOMEM);

	/* prepare to claim, if successful, mark claiming in progress */
	spin_lock(&bdev_lock);

	err = bd_prepare_to_claim(bdev, whole, holder);
	if (err == 0) {
		whole->bd_claiming = holder;
		spin_unlock(&bdev_lock);
		return whole;
	} else {
		spin_unlock(&bdev_lock);
		bdput(whole);
		return ERR_PTR(err);
	}
}
