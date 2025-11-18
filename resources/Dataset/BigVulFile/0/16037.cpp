XmpIteratorPtr xmp_iterator_new(XmpPtr xmp, const char *schema,
                                const char *propName, XmpIterOptions options)
{
    CHECK_PTR(xmp, NULL);
    RESET_ERROR;

    try {
        auto xiter = std::unique_ptr<SXMPIterator>(
            new SXMPIterator(*(SXMPMeta *)xmp, schema, propName, options));

        return reinterpret_cast<XmpIteratorPtr>(xiter.release());
    }
    catch (const XMP_Error &e) {
        set_error(e);
    }

    return NULL;
}
