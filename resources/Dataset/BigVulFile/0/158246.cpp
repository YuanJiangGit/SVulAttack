void RenderWidgetHostImpl::DidStartScrollingViewport() {
  if (view_)
    view_->set_is_currently_scrolling_viewport(true);
}
