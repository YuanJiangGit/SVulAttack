void AppCacheUpdateJob::ContinueHandleManifestFetchCompleted(bool changed) {
  DCHECK(internal_state_ == FETCH_MANIFEST);

  if (!changed) {
    DCHECK(update_type_ == UPGRADE_ATTEMPT);
    internal_state_ = NO_UPDATE;

    FetchMasterEntries();
    MaybeCompleteUpdate();  // if not done, run async 6.9.4 step 7 substeps
    return;
  }

  AppCacheManifest manifest;
  if (!ParseManifest(manifest_url_, manifest_data_.data(),
                     manifest_data_.length(),
                     manifest_has_valid_mime_type_
                         ? PARSE_MANIFEST_ALLOWING_DANGEROUS_FEATURES
                         : PARSE_MANIFEST_PER_STANDARD,
                     manifest)) {
    const char kFormatString[] = "Failed to parse manifest %s";
    const std::string message = base::StringPrintf(kFormatString,
        manifest_url_.spec().c_str());
    HandleCacheFailure(
        blink::mojom::AppCacheErrorDetails(
            message,
            blink::mojom::AppCacheErrorReason::APPCACHE_SIGNATURE_ERROR, GURL(),
            0, false /*is_cross_origin*/),
        MANIFEST_ERROR, GURL());
    VLOG(1) << message;
    return;
  }

  internal_state_ = DOWNLOADING;
  inprogress_cache_ = new AppCache(storage_, storage_->NewCacheId());
  BuildUrlFileList(manifest);
  inprogress_cache_->InitializeWithManifest(&manifest);

  for (const auto& pair : pending_master_entries_) {
    const PendingHosts& hosts = pair.second;
    for (AppCacheHost* host : hosts) {
      host->AssociateIncompleteCache(inprogress_cache_.get(), manifest_url_);
    }
  }

  if (manifest.did_ignore_intercept_namespaces) {
    std::string message(
        "Ignoring the INTERCEPT section of the application cache manifest "
        "because the content type is not text/cache-manifest");
    LogConsoleMessageToAll(message);
  }
  if (manifest.did_ignore_fallback_namespaces) {
    std::string message(
        "Ignoring out of scope FALLBACK entries of the application cache "
        "manifest because the content-type is not text/cache-manifest");
    LogConsoleMessageToAll(message);
  }

  group_->SetUpdateAppCacheStatus(AppCacheGroup::DOWNLOADING);
  NotifyAllAssociatedHosts(
      blink::mojom::AppCacheEventID::APPCACHE_DOWNLOADING_EVENT);
  FetchUrls();
  FetchMasterEntries();
  MaybeCompleteUpdate();  // if not done, continues when async fetches complete
}
