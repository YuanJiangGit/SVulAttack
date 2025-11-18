media_status_t AMediaCodec_delete(AMediaCodec *mData) {
 if (mData != NULL) {
 if (mData->mCodec != NULL) {
            mData->mCodec->release();
            mData->mCodec.clear();
 }

 if (mData->mLooper != NULL) {
 if (mData->mHandler != NULL) {
                mData->mLooper->unregisterHandler(mData->mHandler->id());
 }
            mData->mLooper->stop();
            mData->mLooper.clear();
 }
 delete mData;
 }
 return AMEDIA_OK;
}
