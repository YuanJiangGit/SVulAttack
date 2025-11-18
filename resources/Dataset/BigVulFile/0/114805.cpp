__xmlOutputBufferCreateFilenameValue(void) {
    if (IS_MAIN_THREAD)
	return (&xmlOutputBufferCreateFilenameValue);
    else
	return (&xmlGetGlobalState()->xmlOutputBufferCreateFilenameValue);
}
