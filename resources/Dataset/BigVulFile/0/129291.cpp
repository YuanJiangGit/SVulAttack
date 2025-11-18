void GLES2DecoderImpl::DoGetTexParameteriv(
    GLenum target, GLenum pname, GLint* params) {
  InitTextureMaxAnisotropyIfNeeded(target, pname);
  glGetTexParameteriv(target, pname, params);
}
