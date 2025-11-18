Camera3Device::RequestThread::RequestThread(wp<Camera3Device> parent,
        sp<StatusTracker> statusTracker,
 camera3_device_t *hal3Device,
 bool aeLockAvailable) :
 Thread(/*canCallJava*/false),
        mParent(parent),
        mStatusTracker(statusTracker),
        mHal3Device(hal3Device),
        mId(getId(parent)),
        mReconfigured(false),
        mDoPause(false),
        mPaused(true),
        mFrameNumber(0),
        mLatestRequestId(NAME_NOT_FOUND),
        mCurrentAfTriggerId(0),
        mCurrentPreCaptureTriggerId(0),
        mRepeatingLastFrameNumber(NO_IN_FLIGHT_REPEATING_FRAMES),
        mAeLockAvailable(aeLockAvailable) {
    mStatusId = statusTracker->addComponent();
}
