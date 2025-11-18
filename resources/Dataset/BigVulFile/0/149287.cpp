void DatabaseImpl::IDBThreadHelper::Close() {
  DCHECK(idb_thread_checker_.CalledOnValidThread());
  if (!connection_->IsConnected())
    return;

  connection_->Close();
}
