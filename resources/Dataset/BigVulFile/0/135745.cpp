bool InputMethodController::HasComposition() const {
  return has_composition_ && !composition_range_->collapsed() &&
         composition_range_->IsConnected();
}
