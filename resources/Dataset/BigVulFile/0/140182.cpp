bool GaiaCookieManagerService::ExternalCcResultFetcher::IsRunning() {
  return helper_->gaia_auth_fetcher_ || fetchers_.size() > 0u;
}
