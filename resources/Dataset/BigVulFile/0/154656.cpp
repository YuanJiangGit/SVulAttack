error::Error GLES2DecoderPassthroughImpl::DoFramebufferParameteri(GLenum target,
                                                                  GLenum pname,
                                                                  GLint param) {
  api()->glFramebufferParameteriFn(target, pname, param);
  return error::kNoError;
}
