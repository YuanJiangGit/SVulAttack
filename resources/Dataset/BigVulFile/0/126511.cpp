gfx::Rect TabStripGtk::GetIdealBounds(int index) {
  DCHECK(index >= 0 && index < GetTabCount());
  return tab_data_.at(index).ideal_bounds;
}
