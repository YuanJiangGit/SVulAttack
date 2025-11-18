xmlBufGetAllocationScheme(xmlBufPtr buf) {
    if (buf == NULL) {
#ifdef DEBUG_BUFFER
        xmlGenericError(xmlGenericErrorContext,
		"xmlBufGetAllocationScheme: buf == NULL\n");
#endif
        return(-1);
    }
    return(buf->alloc);
}
