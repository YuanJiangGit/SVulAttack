DragOperation DragController::OperationForLoad(DragData* drag_data,
                                               LocalFrame& local_root) {
  DCHECK(drag_data);
  Document* doc =
      local_root.DocumentAtPoint(LayoutPoint(drag_data->ClientPosition()));

  if (doc &&
      (did_initiate_drag_ || doc->IsPluginDocument() || HasEditableStyle(*doc)))
    return kDragOperationNone;
  return GetDragOperation(drag_data);
}
