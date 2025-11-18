bool OMXNodeInstance::isProhibitedIndex_l(OMX_INDEXTYPE index) {
 static const char *restricted_extensions[] = {
 "OMX.google.android.index.storeMetaDataInBuffers",
 "OMX.google.android.index.storeANWBufferInMetadata",
 "OMX.google.android.index.prepareForAdaptivePlayback",
 "OMX.google.android.index.configureVideoTunnelMode",
 "OMX.google.android.index.useAndroidNativeBuffer2",
 "OMX.google.android.index.useAndroidNativeBuffer",
 "OMX.google.android.index.enableAndroidNativeBuffers",
 "OMX.google.android.index.allocateNativeHandle",
 "OMX.google.android.index.getAndroidNativeBufferUsage",
 };

 if ((index > OMX_IndexComponentStartUnused && index <= OMX_IndexParamStandardComponentRole)
 || (index > OMX_IndexPortStartUnused && index <= OMX_IndexParamCompBufferSupplier)
 || (index > OMX_IndexAudioStartUnused && index <= OMX_IndexConfigAudioChannelVolume)
 || (index > OMX_IndexVideoStartUnused && index <= OMX_IndexConfigVideoNalSize)
 || (index > OMX_IndexCommonStartUnused
 && index <= OMX_IndexConfigCommonTransitionEffect)
 || (index > (OMX_INDEXTYPE)OMX_IndexExtAudioStartUnused
 && index <= (OMX_INDEXTYPE)OMX_IndexParamAudioProfileQuerySupported)
 || (index > (OMX_INDEXTYPE)OMX_IndexExtVideoStartUnused
 && index <= (OMX_INDEXTYPE)OMX_IndexConfigAndroidIntraRefresh)
 || (index > (OMX_INDEXTYPE)OMX_IndexExtOtherStartUnused
 && index <= (OMX_INDEXTYPE)OMX_IndexParamConsumerUsageBits)) {
 return false;
 }

 if (!mQueriedProhibitedExtensions) {
 for (size_t i = 0; i < NELEM(restricted_extensions); ++i) {
            OMX_INDEXTYPE ext;
 if (OMX_GetExtensionIndex(mHandle, (OMX_STRING)restricted_extensions[i], &ext) == OMX_ErrorNone) {
                mProhibitedExtensions.add(ext);
 }
 }
        mQueriedProhibitedExtensions = true;
 }

 return mProhibitedExtensions.indexOf(index) >= 0;
}
