WebContentsImpl* WebContentsImpl::GetOuterWebContents() {
  if (GuestMode::IsCrossProcessFrameGuest(this))
    return node_.outer_web_contents();

  if (browser_plugin_guest_)
    return browser_plugin_guest_->embedder_web_contents();

  return nullptr;
}
