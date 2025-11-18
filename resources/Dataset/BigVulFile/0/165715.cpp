void FileReaderLoader::OnDataPipeReadable(MojoResult result) {
  if (result != MOJO_RESULT_OK) {
    if (!received_all_data_) {
      Failed(FileErrorCode::kNotReadableErr,
             FailureType::kDataPipeNotReadableWithBytesLeft);
    }
    return;
  }

  while (true) {
    uint32_t num_bytes;
    const void* buffer;
    MojoResult result = consumer_handle_->BeginReadData(
        &buffer, &num_bytes, MOJO_READ_DATA_FLAG_NONE);
    if (result == MOJO_RESULT_SHOULD_WAIT) {
      if (!IsSyncLoad())
        return;

      result = mojo::Wait(consumer_handle_.get(), MOJO_HANDLE_SIGNAL_READABLE);
      if (result == MOJO_RESULT_OK)
        continue;
    }
    if (result == MOJO_RESULT_FAILED_PRECONDITION) {
      if (!received_all_data_) {
        Failed(FileErrorCode::kNotReadableErr,
               FailureType::kMojoPipeClosedEarly);
      }
      return;
    }
    if (result != MOJO_RESULT_OK) {
      Failed(FileErrorCode::kNotReadableErr,
             FailureType::kMojoPipeUnexpectedReadError);
      return;
    }

    auto weak_this = weak_factory_.GetWeakPtr();
    OnReceivedData(static_cast<const char*>(buffer), num_bytes);
    if (!weak_this)
      return;

    consumer_handle_->EndReadData(num_bytes);
    if (BytesLoaded() >= total_bytes_) {
      received_all_data_ = true;
      if (received_on_complete_)
        OnFinishLoading();
      return;
    }
  }
}
