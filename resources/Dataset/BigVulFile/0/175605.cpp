NuMediaExtractor::~NuMediaExtractor() {
    releaseTrackSamples();

 for (size_t i = 0; i < mSelectedTracks.size(); ++i) {
 TrackInfo *info = &mSelectedTracks.editItemAt(i);

        CHECK_EQ((status_t)OK, info->mSource->stop());
 }

    mSelectedTracks.clear();
 if (mDataSource != NULL) {
        mDataSource->close();
 }
}
