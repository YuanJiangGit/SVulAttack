static void mddev_detach(struct mddev *mddev)
{
	struct bitmap *bitmap = mddev->bitmap;
	/* wait for behind writes to complete */
	if (bitmap && atomic_read(&bitmap->behind_writes) > 0) {
		printk(KERN_INFO "md:%s: behind writes in progress - waiting to stop.\n",
		       mdname(mddev));
		/* need to kick something here to make sure I/O goes? */
		wait_event(bitmap->behind_wait,
			   atomic_read(&bitmap->behind_writes) == 0);
	}
	if (mddev->pers && mddev->pers->quiesce) {
		mddev->pers->quiesce(mddev, 1);
		mddev->pers->quiesce(mddev, 0);
	}
	md_unregister_thread(&mddev->thread);
	if (mddev->queue)
		blk_sync_queue(mddev->queue); /* the unplug fn references 'conf'*/
}
