void NaClIPCAdapter::SendMessageOnIOThread(scoped_ptr<IPC::Message> message) {
  io_thread_data_.channel_->Send(message.release());
}
