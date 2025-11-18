scoped_refptr<EntryImpl> BackendImpl::ResurrectEntry(
    scoped_refptr<EntryImpl> deleted_entry) {
  if (ENTRY_NORMAL == deleted_entry->entry()->Data()->state) {
    deleted_entry = nullptr;
    stats_.OnEvent(Stats::CREATE_MISS);
    Trace("create entry miss ");
    return NULL;
  }


  eviction_.OnCreateEntry(deleted_entry.get());
  entry_count_++;

  stats_.OnEvent(Stats::RESURRECT_HIT);
  Trace("Resurrect entry hit ");
  return deleted_entry;
}
