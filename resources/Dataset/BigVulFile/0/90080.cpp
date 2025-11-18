size_t ZSTD_initCStream(ZSTD_CStream* zcs, int compressionLevel)
{
    DEBUGLOG(4, "ZSTD_initCStream");
    return ZSTD_initCStream_srcSize(zcs, compressionLevel, ZSTD_CONTENTSIZE_UNKNOWN);
}
