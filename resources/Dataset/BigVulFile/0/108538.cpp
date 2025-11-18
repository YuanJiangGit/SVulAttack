void QuotaManager::SetPersistentHostQuota(const std::string& host,
                                          int64 new_quota,
                                          const HostQuotaCallback& callback) {
  LazyInitialize();
  if (host.empty()) {
    callback.Run(kQuotaErrorNotSupported, host, kStorageTypePersistent, 0);
    return;
  }
  if (new_quota < 0) {
    callback.Run(kQuotaErrorInvalidModification,
                 host, kStorageTypePersistent, -1);
    return;
  }

  if (db_disabled_) {
    callback.Run(kQuotaErrorInvalidAccess,
                 host, kStorageTypePersistent, -1);
    return;
  }

  int64* new_quota_ptr = new int64(new_quota);
  PostTaskAndReplyWithResultForDBThread(
      FROM_HERE,
      base::Bind(&SetPersistentHostQuotaOnDBThread,
                 host,
                 base::Unretained(new_quota_ptr)),
      base::Bind(&QuotaManager::DidSetPersistentHostQuota,
                 weak_factory_.GetWeakPtr(),
                 host,
                 callback,
                 base::Owned(new_quota_ptr)));
}
