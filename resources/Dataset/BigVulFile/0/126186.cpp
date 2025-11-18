bool Browser::GetConstrainedWindowTopCenter(gfx::Point* point) {
  int y = 0;
  if (window_->GetConstrainedWindowTopY(&y)) {
    *point = gfx::Point(window_->GetBounds().width() / 2, y);
    return true;
  }

  return false;
}
