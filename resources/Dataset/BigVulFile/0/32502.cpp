static int tg3_ape_scratchpad_read(struct tg3 *tp, u32 *data, u32 base_off,
				   u32 len)
{
	int err;
	u32 i, bufoff, msgoff, maxlen, apedata;

	if (!tg3_flag(tp, APE_HAS_NCSI))
		return 0;

	apedata = tg3_ape_read32(tp, TG3_APE_SEG_SIG);
	if (apedata != APE_SEG_SIG_MAGIC)
		return -ENODEV;

	apedata = tg3_ape_read32(tp, TG3_APE_FW_STATUS);
	if (!(apedata & APE_FW_STATUS_READY))
		return -EAGAIN;

	bufoff = tg3_ape_read32(tp, TG3_APE_SEG_MSG_BUF_OFF) +
		 TG3_APE_SHMEM_BASE;
	msgoff = bufoff + 2 * sizeof(u32);
	maxlen = tg3_ape_read32(tp, TG3_APE_SEG_MSG_BUF_LEN);

	while (len) {
		u32 length;

		/* Cap xfer sizes to scratchpad limits. */
		length = (len > maxlen) ? maxlen : len;
		len -= length;

		apedata = tg3_ape_read32(tp, TG3_APE_FW_STATUS);
		if (!(apedata & APE_FW_STATUS_READY))
			return -EAGAIN;

		/* Wait for up to 1 msec for APE to service previous event. */
		err = tg3_ape_event_lock(tp, 1000);
		if (err)
			return err;

		apedata = APE_EVENT_STATUS_DRIVER_EVNT |
			  APE_EVENT_STATUS_SCRTCHPD_READ |
			  APE_EVENT_STATUS_EVENT_PENDING;
		tg3_ape_write32(tp, TG3_APE_EVENT_STATUS, apedata);

		tg3_ape_write32(tp, bufoff, base_off);
		tg3_ape_write32(tp, bufoff + sizeof(u32), length);

		tg3_ape_unlock(tp, TG3_APE_LOCK_MEM);
		tg3_ape_write32(tp, TG3_APE_EVENT, APE_EVENT_1);

		base_off += length;

		if (tg3_ape_wait_for_event(tp, 30000))
			return -EAGAIN;

		for (i = 0; length; i += 4, length -= 4) {
			u32 val = tg3_ape_read32(tp, msgoff + i);
			memcpy(data, &val, sizeof(u32));
			data++;
		}
	}

	return 0;
}
