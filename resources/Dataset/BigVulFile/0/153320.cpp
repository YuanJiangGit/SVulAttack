gfx::Size Tab::CalculatePreferredSize() const {
  return gfx::Size(TabStyle::GetStandardWidth(), GetLayoutConstant(TAB_HEIGHT));
}
