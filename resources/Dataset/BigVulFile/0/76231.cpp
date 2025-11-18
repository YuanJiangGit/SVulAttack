static int cdrom_ioctl_eject(struct cdrom_device_info *cdi)
{
	cd_dbg(CD_DO_IOCTL, "entering CDROMEJECT\n");

	if (!CDROM_CAN(CDC_OPEN_TRAY))
		return -ENOSYS;
	if (cdi->use_count != 1 || cdi->keeplocked)
		return -EBUSY;
	if (CDROM_CAN(CDC_LOCK)) {
		int ret = cdi->ops->lock_door(cdi, 0);
		if (ret)
			return ret;
	}

	return cdi->ops->tray_move(cdi, 1);
}
