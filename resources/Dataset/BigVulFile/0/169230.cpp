void FetchHistogramsFromChildProcesses() {
  scoped_refptr<content::MessageLoopRunner> runner = new MessageLoopRunner;

  FetchHistogramsAsynchronously(
      base::ThreadTaskRunnerHandle::Get(), runner->QuitClosure(),
      TestTimeouts::action_max_timeout());
  runner->Run();
}
