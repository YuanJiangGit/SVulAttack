void RTCPeerConnectionHandler::getStats(
    const scoped_refptr<LocalRTCStatsRequest>& request) {
  DCHECK(task_runner_->RunsTasksInCurrentSequence());
  TRACE_EVENT0("webrtc", "RTCPeerConnectionHandler::getStats");

  rtc::scoped_refptr<webrtc::StatsObserver> observer(
      new rtc::RefCountedObject<StatsResponse>(request, task_runner_));

  rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> selector;
  if (request->hasSelector()) {
    auto track_adapter_ref =
        track_adapter_map_->GetLocalTrackAdapter(request->component());
    if (!track_adapter_ref) {
      track_adapter_ref =
          track_adapter_map_->GetRemoteTrackAdapter(request->component());
    }
    if (track_adapter_ref)
      selector = track_adapter_ref->webrtc_track();
  }

  GetStats(observer, webrtc::PeerConnectionInterface::kStatsOutputLevelStandard,
           std::move(selector));
}
