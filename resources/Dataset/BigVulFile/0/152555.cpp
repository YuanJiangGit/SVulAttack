void RenderFrameImpl::UpdateUserActivationState(
    blink::UserActivationUpdateType update_type) {
  Send(new FrameHostMsg_UpdateUserActivationState(routing_id_, update_type));
}
