void GLES2DecoderImpl::DoTexParameteriv(GLenum target,
                                        GLenum pname,
                                        const volatile GLint* params) {
  TextureRef* texture = texture_manager()->GetTextureInfoForTarget(
      &state_, target);
  if (!texture) {
    LOCAL_SET_GL_ERROR(
        GL_INVALID_VALUE, "glTexParameteriv", "unknown texture");
    return;
  }

  texture_manager()->SetParameteri("glTexParameteriv", error_state_.get(),
                                   texture, pname, *params);
}
