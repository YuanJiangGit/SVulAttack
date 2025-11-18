gfx::Rect BrowserNonClientFrameViewAura::GetBoundsForTabStrip(
    views::View* tabstrip) const {
  if (!tabstrip)
    return gfx::Rect();
  int tabstrip_x =
      avatar_button() ?
      (avatar_button()->bounds().right() + kAvatarSideSpacing) :
      kTabstripLeftSpacing;
  int tabstrip_width =
      maximize_button_->x() - kTabstripRightSpacing - tabstrip_x;
  return gfx::Rect(tabstrip_x,
                   GetHorizontalTabStripVerticalOffset(false),
                   std::max(0, tabstrip_width),
                   tabstrip->GetPreferredSize().height());
}
