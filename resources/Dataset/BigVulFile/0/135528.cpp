bool Editor::CanRedo() {
  return undo_stack_->CanRedo();
}
