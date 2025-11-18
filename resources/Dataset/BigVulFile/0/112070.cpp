bool SyncManager::EncryptEverythingEnabledForTest() const {
  ReadTransaction trans(FROM_HERE, GetUserShare());
  return trans.GetCryptographer()->encrypt_everything();
}
