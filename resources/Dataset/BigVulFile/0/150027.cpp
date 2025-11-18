LayerTreeHostImpl::ScrollbarAnimationControllerForId(
    int scroll_layer_id) const {
  if (InnerViewportScrollLayer() && OuterViewportScrollLayer() &&
      scroll_layer_id == InnerViewportScrollLayer()->id())
    scroll_layer_id = OuterViewportScrollLayer()->id();
  auto i = scrollbar_animation_controllers_.find(scroll_layer_id);
  if (i == scrollbar_animation_controllers_.end())
    return nullptr;
  return i->second.get();
}
