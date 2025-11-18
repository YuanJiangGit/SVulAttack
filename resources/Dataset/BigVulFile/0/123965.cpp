void RenderViewImpl::didCancelCompositionOnSelectionChange() {
  Send(new ViewHostMsg_ImeCancelComposition(routing_id()));
}
