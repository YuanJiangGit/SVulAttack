void TabStrip::OnMouseEventInTab(views::View* source,
                                 const ui::MouseEvent& event) {
  UpdateStackedLayoutFromMouseEvent(source, event);
}
