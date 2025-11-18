void SocketStream::SetUserData(const void* key, UserData* data) {
  user_data_[key] = linked_ptr<UserData>(data);
}
