status_t NuPlayer::GenericSource::setDataSource(
 const sp<IMediaHTTPService> &httpService,
 const char *url,
 const KeyedVector<String8, String8> *headers) {
    resetDataSource();

    mHTTPService = httpService;
    mUri = url;

 if (headers) {
        mUriHeaders = *headers;
 }

 return OK;
}
