void SynchronizeVisualPropertiesMessageFilter::ResetRectRunLoop() {
  last_rect_ = gfx::Rect();
  screen_space_rect_run_loop_.reset(new base::RunLoop);
  screen_space_rect_received_ = false;
}
