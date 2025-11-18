status_t SampleIterator::findChunkRange(uint32_t sampleIndex) {
    CHECK(sampleIndex >= mFirstChunkSampleIndex);

 while (sampleIndex >= mStopChunkSampleIndex) {
 if (mSampleToChunkIndex == mTable->mNumSampleToChunkOffsets) {
 return ERROR_OUT_OF_RANGE;
 }

        mFirstChunkSampleIndex = mStopChunkSampleIndex;

 const SampleTable::SampleToChunkEntry *entry =
 &mTable->mSampleToChunkEntries[mSampleToChunkIndex];

        mFirstChunk = entry->startChunk;
        mSamplesPerChunk = entry->samplesPerChunk;
        mChunkDesc = entry->chunkDesc;

 if (mSampleToChunkIndex + 1 < mTable->mNumSampleToChunkOffsets) {
            mStopChunk = entry[1].startChunk;

            mStopChunkSampleIndex =
                mFirstChunkSampleIndex
 + (mStopChunk - mFirstChunk) * mSamplesPerChunk;
 } else {
            mStopChunk = 0xffffffff;
            mStopChunkSampleIndex = 0xffffffff;
 }

 ++mSampleToChunkIndex;
 }

 return OK;
}
