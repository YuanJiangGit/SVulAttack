status_t Camera2Client::syncWithDevice() {
    ATRACE_CALL();
 const nsecs_t kMaxSyncTimeout = 500000000; // 500 ms
 status_t res;

 int32_t activeRequestId = mStreamingProcessor->getActiveRequestId();
 if (activeRequestId == 0) return OK;

    res = mDevice->waitUntilRequestReceived(activeRequestId, kMaxSyncTimeout);
 if (res == TIMED_OUT) {
        ALOGE("%s: Camera %d: Timed out waiting sync with HAL",
                __FUNCTION__, mCameraId);
 } else if (res != OK) {
        ALOGE("%s: Camera %d: Error while waiting to sync with HAL",
                __FUNCTION__, mCameraId);
 }
 return res;
}
