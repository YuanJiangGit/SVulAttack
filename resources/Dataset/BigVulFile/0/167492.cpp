MojoResult DataPipeProducerDispatcher::Close() {
  base::AutoLock lock(lock_);
  DVLOG(1) << "Closing data pipe producer " << pipe_id_;
  return CloseNoLock();
}
