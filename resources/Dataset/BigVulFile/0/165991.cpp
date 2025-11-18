bool IsRemoteStream(
    const std::vector<std::unique_ptr<RTCRtpReceiver>>& rtp_receivers,
    const std::string& stream_id) {
  for (const auto& receiver : rtp_receivers) {
    for (const auto& receiver_stream_id : receiver->state().stream_ids()) {
      if (stream_id == receiver_stream_id)
        return true;
    }
  }
  return false;
}
