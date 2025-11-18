bool GLES2DecoderImpl::HasPollingWork() const {
  return deschedule_until_finished_fences_.size() >= 2;
}
