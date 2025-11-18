  virtual void OnMessage(net::WebSocket* socket, const std::string& msg) {
    events_.push_back(
        WebSocketEvent(WebSocketEvent::EVENT_MESSAGE, socket, msg));
    if (onmessage_)
      onmessage_->Run(&events_.back());
  }
