void RenderFrameImpl::didClearWindowObject(blink::WebLocalFrame* frame,
                                           int world_id) {
  DCHECK(!frame_ || frame_ == frame);

  render_view_->didClearWindowObject(frame, world_id);

  if (world_id)
    return;

  if (render_view_->GetEnabledBindings() & BINDINGS_POLICY_DOM_AUTOMATION)
    DomAutomationController::Install(this, frame);

  FOR_EACH_OBSERVER(RenderFrameObserver, observers_,
                    DidClearWindowObject(world_id));
}
