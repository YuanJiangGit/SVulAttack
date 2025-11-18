void WebContentsImpl::AudioContextPlaybackStarted(RenderFrameHost* host,
                                                  int context_id) {
  WebContentsObserver::AudioContextId audio_context_id(host, context_id);
  for (auto& observer : observers_)
    observer.AudioContextPlaybackStarted(audio_context_id);
}
