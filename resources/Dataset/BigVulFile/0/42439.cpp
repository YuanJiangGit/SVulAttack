void md_integrity_add_rdev(struct md_rdev *rdev, struct mddev *mddev)
{
	struct blk_integrity *bi_rdev;
	struct blk_integrity *bi_mddev;

	if (!mddev->gendisk)
		return;

	bi_rdev = bdev_get_integrity(rdev->bdev);
	bi_mddev = blk_get_integrity(mddev->gendisk);

	if (!bi_mddev) /* nothing to do */
		return;
	if (rdev->raid_disk < 0) /* skip spares */
		return;
	if (bi_rdev && blk_integrity_compare(mddev->gendisk,
					     rdev->bdev->bd_disk) >= 0)
		return;
	printk(KERN_NOTICE "disabling data integrity on %s\n", mdname(mddev));
	blk_integrity_unregister(mddev->gendisk);
}
