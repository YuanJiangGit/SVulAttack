  void Wait() {
    if (web_contents()->GetController().GetVisibleEntry())
      return;
    run_loop_.Run();
  }
