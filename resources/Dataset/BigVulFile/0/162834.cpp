void ContextState::UpdatePackParameters() const {
  if (!feature_info_->IsES3Capable())
    return;
  if (bound_pixel_pack_buffer.get()) {
    api()->glPixelStoreiFn(GL_PACK_ROW_LENGTH, pack_row_length);
  } else {
    api()->glPixelStoreiFn(GL_PACK_ROW_LENGTH, 0);
  }
}
