void MediaStreamManager::UnregisterNativeLogCallback(int renderer_host_id) {
  DCHECK_CURRENTLY_ON(BrowserThread::IO);
  MediaStreamManager* msm = g_media_stream_manager_tls_ptr.Pointer()->Get();
  if (!msm) {
    DLOG(ERROR) << "No MediaStreamManager on the IO thread.";
    return;
  }

  msm->DoNativeLogCallbackUnregistration(renderer_host_id);
}
