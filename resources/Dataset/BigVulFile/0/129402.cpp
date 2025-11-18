error::Error GLES2DecoderImpl::HandleDrawArrays(
    uint32 immediate_data_size, const cmds::DrawArrays& c) {
  return DoDrawArrays("glDrawArrays",
                      false,
                      static_cast<GLenum>(c.mode),
                      static_cast<GLint>(c.first),
                      static_cast<GLsizei>(c.count),
                      0);
}
