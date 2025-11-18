static int digi_port_probe(struct usb_serial_port *port)
{
	return digi_port_init(port, port->port_number);
}
