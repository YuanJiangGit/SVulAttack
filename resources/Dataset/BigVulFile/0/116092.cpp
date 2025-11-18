 void SyncManager::SyncInternal::OnIPAddressChangedImpl() {
  DCHECK(thread_checker_.CalledOnValidThread());
  if (scheduler())
    scheduler()->OnConnectionStatusChange();
}
