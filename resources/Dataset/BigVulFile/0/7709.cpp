ssize_t pdu_unmarshal(V9fsPDU *pdu, size_t offset, const char *fmt, ...)
{
    ssize_t ret;
    va_list ap;

    va_start(ap, fmt);
    ret = virtio_pdu_vunmarshal(pdu, offset, fmt, ap);
    va_end(ap);

    return ret;
}
