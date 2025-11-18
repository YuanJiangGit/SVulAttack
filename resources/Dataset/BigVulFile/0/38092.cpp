static void firm_setup_port(struct tty_struct *tty)
{
	struct usb_serial_port *port = tty->driver_data;
	struct device *dev = &port->dev;
	struct whiteheat_port_settings port_settings;
	unsigned int cflag = tty->termios.c_cflag;

	port_settings.port = port->port_number + 1;

	/* get the byte size */
	switch (cflag & CSIZE) {
	case CS5:	port_settings.bits = 5;   break;
	case CS6:	port_settings.bits = 6;   break;
	case CS7:	port_settings.bits = 7;   break;
	default:
	case CS8:	port_settings.bits = 8;   break;
	}
	dev_dbg(dev, "%s - data bits = %d\n", __func__, port_settings.bits);

	/* determine the parity */
	if (cflag & PARENB)
		if (cflag & CMSPAR)
			if (cflag & PARODD)
				port_settings.parity = WHITEHEAT_PAR_MARK;
			else
				port_settings.parity = WHITEHEAT_PAR_SPACE;
		else
			if (cflag & PARODD)
				port_settings.parity = WHITEHEAT_PAR_ODD;
			else
				port_settings.parity = WHITEHEAT_PAR_EVEN;
	else
		port_settings.parity = WHITEHEAT_PAR_NONE;
	dev_dbg(dev, "%s - parity = %c\n", __func__, port_settings.parity);

	/* figure out the stop bits requested */
	if (cflag & CSTOPB)
		port_settings.stop = 2;
	else
		port_settings.stop = 1;
	dev_dbg(dev, "%s - stop bits = %d\n", __func__, port_settings.stop);

	/* figure out the flow control settings */
	if (cflag & CRTSCTS)
		port_settings.hflow = (WHITEHEAT_HFLOW_CTS |
						WHITEHEAT_HFLOW_RTS);
	else
		port_settings.hflow = WHITEHEAT_HFLOW_NONE;
	dev_dbg(dev, "%s - hardware flow control = %s %s %s %s\n", __func__,
	    (port_settings.hflow & WHITEHEAT_HFLOW_CTS) ? "CTS" : "",
	    (port_settings.hflow & WHITEHEAT_HFLOW_RTS) ? "RTS" : "",
	    (port_settings.hflow & WHITEHEAT_HFLOW_DSR) ? "DSR" : "",
	    (port_settings.hflow & WHITEHEAT_HFLOW_DTR) ? "DTR" : "");

	/* determine software flow control */
	if (I_IXOFF(tty))
		port_settings.sflow = WHITEHEAT_SFLOW_RXTX;
	else
		port_settings.sflow = WHITEHEAT_SFLOW_NONE;
	dev_dbg(dev, "%s - software flow control = %c\n", __func__, port_settings.sflow);

	port_settings.xon = START_CHAR(tty);
	port_settings.xoff = STOP_CHAR(tty);
	dev_dbg(dev, "%s - XON = %2x, XOFF = %2x\n", __func__, port_settings.xon, port_settings.xoff);

	/* get the baud rate wanted */
	port_settings.baud = tty_get_baud_rate(tty);
	dev_dbg(dev, "%s - baud rate = %d\n", __func__, port_settings.baud);

	/* fixme: should set validated settings */
	tty_encode_baud_rate(tty, port_settings.baud, port_settings.baud);
	/* handle any settings that aren't specified in the tty structure */
	port_settings.lloop = 0;

	/* now send the message to the device */
	firm_send_command(port, WHITEHEAT_SETUP_PORT,
			(__u8 *)&port_settings, sizeof(port_settings));
}
