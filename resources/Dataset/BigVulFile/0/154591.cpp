error::Error GLES2DecoderPassthroughImpl::DoClearStencil(GLint s) {
  api()->glClearStencilFn(s);
  return error::kNoError;
}
