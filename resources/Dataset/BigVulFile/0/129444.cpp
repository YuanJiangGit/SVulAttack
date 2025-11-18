error::Error GLES2DecoderImpl::HandleWaitAsyncTexImage2DCHROMIUM(
    uint32 immediate_data_size, const cmds::WaitAsyncTexImage2DCHROMIUM& c) {
  TRACE_EVENT0("gpu", "GLES2DecoderImpl::HandleWaitAsyncTexImage2DCHROMIUM");
  GLenum target = static_cast<GLenum>(c.target);

  if (GL_TEXTURE_2D != target) {
    LOCAL_SET_GL_ERROR(
        GL_INVALID_ENUM, "glWaitAsyncTexImage2DCHROMIUM", "target");
    return error::kNoError;
  }
  TextureRef* texture_ref = texture_manager()->GetTextureInfoForTarget(
      &state_, target);
  if (!texture_ref) {
      LOCAL_SET_GL_ERROR(
          GL_INVALID_OPERATION,
          "glWaitAsyncTexImage2DCHROMIUM", "unknown texture");
    return error::kNoError;
  }
  AsyncPixelTransferDelegate* delegate =
      async_pixel_transfer_manager_->GetPixelTransferDelegate(texture_ref);
  if (!delegate) {
      LOCAL_SET_GL_ERROR(
          GL_INVALID_OPERATION,
          "glWaitAsyncTexImage2DCHROMIUM", "No async transfer started");
    return error::kNoError;
  }
  delegate->WaitForTransferCompletion();
  ProcessFinishedAsyncTransfers();
  return error::kNoError;
}
