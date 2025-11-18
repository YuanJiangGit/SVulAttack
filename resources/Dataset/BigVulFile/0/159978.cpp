void DiskCacheBackendTest::BackendInvalidEntryWithLoad() {
  SetMask(0xf);
  SetMaxSize(0x100000);
  InitCache();

  int seed = static_cast<int>(Time::Now().ToInternalValue());
  srand(seed);

  const int kNumEntries = 100;
  disk_cache::Entry* entries[kNumEntries];
  for (int i = 0; i < kNumEntries; i++) {
    std::string key = GenerateKey(true);
    ASSERT_THAT(CreateEntry(key, &entries[i]), IsOk());
  }
  EXPECT_EQ(kNumEntries, cache_->GetEntryCount());

  for (int i = 0; i < kNumEntries; i++) {
    int source1 = rand() % kNumEntries;
    int source2 = rand() % kNumEntries;
    disk_cache::Entry* temp = entries[source1];
    entries[source1] = entries[source2];
    entries[source2] = temp;
  }

  std::string keys[kNumEntries];
  for (int i = 0; i < kNumEntries; i++) {
    keys[i] = entries[i]->GetKey();
    if (i < kNumEntries / 2)
      entries[i]->Close();
  }

  SimulateCrash();

  for (int i = kNumEntries / 2; i < kNumEntries; i++) {
    disk_cache::Entry* entry;
    EXPECT_NE(net::OK, OpenEntry(keys[i], &entry));
  }

  for (int i = 0; i < kNumEntries / 2; i++) {
    disk_cache::Entry* entry;
    ASSERT_THAT(OpenEntry(keys[i], &entry), IsOk());
    entry->Close();
  }

  EXPECT_EQ(kNumEntries / 2, cache_->GetEntryCount());
}
