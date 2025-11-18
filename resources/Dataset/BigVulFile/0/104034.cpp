bool GLES2DecoderImpl::DoIsProgram(GLuint client_id) {
  return GetProgramInfo(client_id) != NULL;
}
