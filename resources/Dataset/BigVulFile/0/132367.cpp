void VirtualKeyboardController::OnMaximizeModeEnded() {
  if (!IsSmartVirtualKeyboardEnabled()) {
    SetKeyboardEnabled(false);
  } else {
    UpdateKeyboardEnabled();
  }
}
