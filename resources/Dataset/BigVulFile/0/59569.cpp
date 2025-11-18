commentDebug(void *ctx ATTRIBUTE_UNUSED, const xmlChar *value)
{
    callbacks++;
    if (quiet)
	return;
    fprintf(SAXdebug, "SAX.comment(%s)\n", value);
}
