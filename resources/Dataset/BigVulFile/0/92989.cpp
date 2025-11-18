process_colour_pointer_pdu(STREAM s)
{
	logger(Protocol, Debug, "%s()", __func__);

	process_colour_pointer_common(s, 24);
}
