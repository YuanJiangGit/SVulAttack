status_t Camera3Device::addDummyStreamLocked() {
    ATRACE_CALL();
 status_t res;

 if (mDummyStreamId != NO_STREAM) {
        SET_ERR_L("%s: Camera %d: A dummy stream already exists!",
                __FUNCTION__, mId);
 return INVALID_OPERATION;
 }

    ALOGV("%s: Camera %d: Adding a dummy stream", __FUNCTION__, mId);

    sp<Camera3OutputStreamInterface> dummyStream =
 new Camera3DummyStream(mNextStreamId);

    res = mOutputStreams.add(mNextStreamId, dummyStream);
 if (res < 0) {
        SET_ERR_L("Can't add dummy stream to set: %s (%d)", strerror(-res), res);
 return res;
 }

    mDummyStreamId = mNextStreamId;
    mNextStreamId++;

 return OK;
}
