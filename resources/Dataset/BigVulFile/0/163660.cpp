xmlBufCCat(xmlBufPtr buf, const char *str) {
    const char *cur;

    if ((buf == NULL) || (buf->error))
        return(-1);
    CHECK_COMPAT(buf)
    if (buf->alloc == XML_BUFFER_ALLOC_IMMUTABLE) return -1;
    if (str == NULL) {
#ifdef DEBUG_BUFFER
        xmlGenericError(xmlGenericErrorContext,
		"xmlBufCCat: str == NULL\n");
#endif
	return -1;
    }
    for (cur = str;*cur != 0;cur++) {
        if (buf->use  + 10 >= buf->size) {
            if (!xmlBufResize(buf, buf->use+10)){
		xmlBufMemoryError(buf, "growing buffer");
                return XML_ERR_NO_MEMORY;
            }
        }
        buf->content[buf->use++] = *cur;
    }
    buf->content[buf->use] = 0;
    UPDATE_COMPAT(buf)
    return 0;
}
