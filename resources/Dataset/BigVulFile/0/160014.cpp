int BackendImpl::CreateEntry(const std::string& key, Entry** entry,
                             const CompletionCallback& callback) {
  DCHECK(!callback.is_null());
  background_queue_.CreateEntry(key, entry, callback);
  return net::ERR_IO_PENDING;
}
