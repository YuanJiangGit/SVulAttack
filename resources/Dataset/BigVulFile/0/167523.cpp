void WebMediaPlayerImpl::MaybeSendOverlayInfoToDecoder() {
  if (!provide_overlay_info_cb_)
    return;

  if (overlay_mode_ == OverlayMode::kUseAndroidOverlay) {
    if (overlay_routing_token_is_pending_)
      return;

    overlay_info_.routing_token = overlay_routing_token_;
  }

  if (decoder_requires_restart_for_overlay_) {
    base::ResetAndReturn(&provide_overlay_info_cb_).Run(overlay_info_);
  } else {
    provide_overlay_info_cb_.Run(overlay_info_);
  }
}
