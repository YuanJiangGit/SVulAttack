void GLES2DecoderImpl::DoCompressedTexSubImage2D(
  GLenum target,
  GLint level,
  GLint xoffset,
  GLint yoffset,
  GLsizei width,
  GLsizei height,
  GLenum format,
  GLsizei image_size,
  const void * data) {
  TextureManager::TextureInfo* info = GetTextureInfoForTarget(target);
  if (!info) {
    SetGLError(GL_INVALID_OPERATION,
               "glCompressedTexSubImage2D", "unknown texture for target");
    return;
  }
  GLenum type = 0;
  GLenum internal_format = 0;
  if (!info->GetLevelType(target, level, &type, &internal_format)) {
    SetGLError(
        GL_INVALID_OPERATION,
        "glCompressedTexSubImage2D", "level does not exist.");
    return;
  }
  if (internal_format != format) {
    SetGLError(
        GL_INVALID_OPERATION,
        "glCompressedTexSubImage2D", "format does not match internal format.");
    return;
  }
  if (!info->ValidForTexture(
      target, level, xoffset, yoffset, width, height, format, type)) {
    SetGLError(GL_INVALID_VALUE,
               "glCompressedTexSubImage2D", "bad dimensions.");
    return;
  }

  if (!ValidateCompressedTexFuncData(
      "glCompressedTexSubImage2D", width, height, format, image_size) ||
      !ValidateCompressedTexSubDimensions(
      "glCompressedTexSubImage2D",
      target, level, xoffset, yoffset, width, height, format, info)) {
    return;
  }


  glCompressedTexSubImage2D(
      target, level, xoffset, yoffset, width, height, format, image_size, data);
}
