bool TextureManager::TextureInfo::IsLevelCleared(GLenum target, GLint level) {
  size_t face_index = GLTargetToFaceIndex(target);
  if (face_index >= level_infos_.size() ||
      level >= static_cast<GLint>(level_infos_[face_index].size())) {
    return true;
  }

  TextureInfo::LevelInfo& info = level_infos_[face_index][level];

  return info.cleared;
}
