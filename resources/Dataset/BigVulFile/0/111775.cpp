void SyncBackendHost::AddExperimentalTypes() {
  CHECK(initialized());
  Experiments experiments;
  if (core_->sync_manager()->ReceivedExperiment(&experiments))
    frontend_->OnExperimentsChanged(experiments);
}
