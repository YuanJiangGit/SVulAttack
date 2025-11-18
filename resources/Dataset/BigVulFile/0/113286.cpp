void PanelBrowserView::Deactivate() {
  if (!IsActive())
    return;

#if defined(OS_WIN) && !defined(USE_AURA)
  gfx::NativeWindow native_window = NULL;
  BrowserWindow* browser_window =
      panel_->manager()->GetNextBrowserWindowToActivate(GetPanelBrowser());
  if (browser_window)
    native_window = browser_window->GetNativeHandle();
  else
    native_window = ::GetDesktopWindow();
  if (native_window)
    ::SetForegroundWindow(native_window);
  else
    ::SetFocus(NULL);
#else
  NOTIMPLEMENTED();
  BrowserView::Deactivate();
#endif
}
