bool DOMMessageQueue::PopMessage(std::string* message) {
  DCHECK(message);
  if (renderer_crashed_ || message_queue_.empty())
    return false;
  *message = message_queue_.front();
  message_queue_.pop();
  return true;
}
