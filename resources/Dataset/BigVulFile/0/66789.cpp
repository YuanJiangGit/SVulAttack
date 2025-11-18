static int usb_cypress_writemem(struct usb_device *udev,u16 addr,u8 *data, u8 len)
{
	return usb_control_msg(udev, usb_sndctrlpipe(udev,0),
			0xa0, USB_TYPE_VENDOR, addr, 0x00, data, len, 5000);
}
