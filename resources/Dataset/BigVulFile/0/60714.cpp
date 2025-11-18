static inline void tower_delete (struct lego_usb_tower *dev)
{
	tower_abort_transfers (dev);

	/* free data structures */
	usb_free_urb(dev->interrupt_in_urb);
	usb_free_urb(dev->interrupt_out_urb);
	kfree (dev->read_buffer);
	kfree (dev->interrupt_in_buffer);
	kfree (dev->interrupt_out_buffer);
	kfree (dev);
}
