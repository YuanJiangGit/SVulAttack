void RenderViewImpl::OnExtendSelectionAndDelete(int before, int after) {
  if (!webview())
    return;
  DCHECK(!handling_ime_event_);
  handling_ime_event_ = true;
  webview()->extendSelectionAndDelete(before, after);
  handling_ime_event_ = false;
  UpdateTextInputState(DO_NOT_SHOW_IME);
}
