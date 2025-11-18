bool RootWindow::DispatchGestureEvent(GestureEvent* event) {
  DispatchHeldMouseMove();

  Window* target = capture_window_;
  if (!target) {
    target = ConsumerToWindow(
        gesture_recognizer_->GetTargetForGestureEvent(event));
    if (!target)
      return false;
  }

  if (target) {
    GestureEvent translated_event(*event, this, target);
    ui::GestureStatus status = ProcessGestureEvent(target, &translated_event);
    return status != ui::GESTURE_STATUS_UNKNOWN;
  }

  return false;
}
