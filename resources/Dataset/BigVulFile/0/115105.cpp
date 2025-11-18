void GetData(GtkClipboard* clipboard,
             GtkSelectionData* selection_data,
             guint info,
             gpointer user_data) {
  Clipboard::TargetMap* data_map =
      reinterpret_cast<Clipboard::TargetMap*>(user_data);

  std::string target_string = GdkAtomToString(selection_data->target);
  Clipboard::TargetMap::iterator iter = data_map->find(target_string);

  if (iter == data_map->end())
    return;

  if (target_string == kMimeTypeBitmap) {
    gtk_selection_data_set_pixbuf(selection_data,
        reinterpret_cast<GdkPixbuf*>(iter->second.first));
  } else {
    gtk_selection_data_set(selection_data, selection_data->target, 8,
                           reinterpret_cast<guchar*>(iter->second.first),
                           iter->second.second);
  }
}
