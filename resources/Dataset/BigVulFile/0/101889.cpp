int Browser::tab_count() const {
  return tab_handler_->GetTabStripModel()->count();
}
