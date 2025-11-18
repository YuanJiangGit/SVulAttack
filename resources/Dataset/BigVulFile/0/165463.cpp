void Document::UpdateStyleAndLayoutIgnorePendingStylesheets(
    Document::RunPostLayoutTasks run_post_layout_tasks) {
  LocalFrameView* local_view = View();
  if (local_view)
    local_view->WillStartForcedLayout();
  if (!RuntimeEnabledFeatures::CSSInBodyDoesNotBlockPaintEnabled())
    UpdateStyleAndLayoutTreeIgnorePendingStylesheets();
  UpdateStyleAndLayout();

  if (local_view) {
    if (run_post_layout_tasks == kRunPostLayoutTasksSynchronously)
      local_view->FlushAnyPendingPostLayoutTasks();

    local_view->DidFinishForcedLayout();
  }
}
