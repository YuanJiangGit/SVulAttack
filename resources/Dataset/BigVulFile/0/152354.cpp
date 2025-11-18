blink::WebURLRequest::PreviewsState RenderFrameImpl::GetPreviewsStateForFrame()
    const {
  PreviewsState disabled_state = previews_state_ & kDisabledPreviewsBits;
  if (disabled_state) {
    DCHECK(!(previews_state_ & ~kDisabledPreviewsBits)) << previews_state_;
    return disabled_state;
  }
  return static_cast<WebURLRequest::PreviewsState>(previews_state_);
}
