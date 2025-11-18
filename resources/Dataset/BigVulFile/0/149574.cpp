  void WaitUntilHostLookedUp(const std::string& host) {
    wait_event_ = WaitEvent::kDns;
    DCHECK(waiting_on_dns_.empty());
    waiting_on_dns_ = host;
    Wait();
  }
