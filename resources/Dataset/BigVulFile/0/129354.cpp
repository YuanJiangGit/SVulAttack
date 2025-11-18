bool GLES2DecoderImpl::GenFramebuffersHelper(
    GLsizei n, const GLuint* client_ids) {
  for (GLsizei ii = 0; ii < n; ++ii) {
    if (GetFramebuffer(client_ids[ii])) {
      return false;
    }
  }
  scoped_ptr<GLuint[]> service_ids(new GLuint[n]);
  glGenFramebuffersEXT(n, service_ids.get());
  for (GLsizei ii = 0; ii < n; ++ii) {
    CreateFramebuffer(client_ids[ii], service_ids[ii]);
  }
  return true;
}
