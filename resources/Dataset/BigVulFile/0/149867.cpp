void LayerTreeHost::UnregisterLayer(Layer* layer) {
  DCHECK(LayerById(layer->id()));
  DCHECK(!in_paint_layer_contents_);
  if (layer->element_id()) {
    mutator_host_->UnregisterElement(layer->element_id(),
                                     ElementListType::ACTIVE);
  }
  RemoveLayerShouldPushProperties(layer);
  layer_id_map_.erase(layer->id());
}
