void PrintRenderFrameHelper::DidFinishLoad() {
  is_loading_ = false;
  if (!on_stop_loading_closure_.is_null()) {
    on_stop_loading_closure_.Run();
    on_stop_loading_closure_.Reset();
  }
}
