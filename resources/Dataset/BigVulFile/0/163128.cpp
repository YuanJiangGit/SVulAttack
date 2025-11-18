void SaveBlobStatusAndFiles(BlobStatus* status_ptr,
                            std::vector<FileCreationInfo>* files_ptr,
                            BlobStatus status,
                            std::vector<FileCreationInfo> files) {
  EXPECT_FALSE(BlobStatusIsError(status));
  *status_ptr = status;
  for (FileCreationInfo& info : files) {
    files_ptr->push_back(std::move(info));
  }
}
