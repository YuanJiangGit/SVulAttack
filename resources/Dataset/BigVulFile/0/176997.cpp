void InputDispatcher::resumeAfterTargetsNotReadyTimeoutLocked(nsecs_t newTimeout,
 const sp<InputChannel>& inputChannel) {
 if (newTimeout > 0) {
        mInputTargetWaitTimeoutTime = now() + newTimeout;
 } else {
        mInputTargetWaitTimeoutExpired = true;

 if (inputChannel.get()) {
 ssize_t connectionIndex = getConnectionIndexLocked(inputChannel);
 if (connectionIndex >= 0) {
                sp<Connection> connection = mConnectionsByFd.valueAt(connectionIndex);
                sp<InputWindowHandle> windowHandle = connection->inputWindowHandle;

 if (windowHandle != NULL) {
 const InputWindowInfo* info = windowHandle->getInfo();
 if (info) {
 ssize_t stateIndex = mTouchStatesByDisplay.indexOfKey(info->displayId);
 if (stateIndex >= 0) {
                            mTouchStatesByDisplay.editValueAt(stateIndex).removeWindow(
                                    windowHandle);
 }
 }
 }

 if (connection->status == Connection::STATUS_NORMAL) {
 CancelationOptions options(CancelationOptions::CANCEL_ALL_EVENTS,
 "application not responding");
                    synthesizeCancelationEventsForConnectionLocked(connection, options);
 }
 }
 }
 }
}
