void DelegatedFrameHost::OnLostResources() {
  EvictDelegatedFrame();
  idle_frame_subscriber_textures_.clear();
  yuv_readback_pipeline_.reset();
}
