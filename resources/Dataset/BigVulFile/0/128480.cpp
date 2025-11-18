void ShellSurface::EndDrag(bool revert) {
  DCHECK(widget_);
  DCHECK(resizer_);

  bool was_resizing = IsResizing();

  if (revert)
    resizer_->RevertDrag();
  else
    resizer_->CompleteDrag();

  ash::Shell::GetInstance()->RemovePreTargetHandler(this);
  widget_->GetNativeWindow()->ReleaseCapture();
  resizer_.reset();

  if (was_resizing)
    Configure();

  UpdateWidgetBounds();
}
