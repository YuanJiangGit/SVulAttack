void ProfileSyncService::OnClearServerDataFailed() {
  clear_server_data_timer_.Stop();

  if (clear_server_data_state_ != CLEAR_SUCCEEDED &&
      clear_server_data_state_ != CLEAR_FAILED) {
    clear_server_data_state_ = CLEAR_FAILED;
    NotifyObservers();
  }
}
