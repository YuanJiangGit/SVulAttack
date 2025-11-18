void RenderProcessHostImpl::OnShutdownRequest() {
  if (pending_views_ || run_renderer_in_process() || GetActiveViewCount() > 0)
    return;

  for (auto& observer : observers_)
    observer.RenderProcessWillExit(this);

  Send(new ChildProcessMsg_Shutdown());
}
