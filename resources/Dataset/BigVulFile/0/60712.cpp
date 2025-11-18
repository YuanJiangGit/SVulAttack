static void tower_abort_transfers (struct lego_usb_tower *dev)
{
	if (dev == NULL)
		return;

	/* shutdown transfer */
	if (dev->interrupt_in_running) {
		dev->interrupt_in_running = 0;
		mb();
		if (dev->udev)
			usb_kill_urb (dev->interrupt_in_urb);
	}
	if (dev->interrupt_out_busy && dev->udev)
		usb_kill_urb(dev->interrupt_out_urb);
}
