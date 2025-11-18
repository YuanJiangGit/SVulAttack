bool ChromeContentBrowserClient::AllowWorkerIndexedDB(
    const GURL& url,
    content::ResourceContext* context,
    const std::vector<content::GlobalFrameRoutingId>& render_frames) {
  DCHECK_CURRENTLY_ON(BrowserThread::IO);
  ProfileIOData* io_data = ProfileIOData::FromResourceContext(context);
  content_settings::CookieSettings* cookie_settings =
      io_data->GetCookieSettings();
  bool allow = cookie_settings->IsCookieAccessAllowed(url, url);

  for (const auto& it : render_frames) {
    base::PostTaskWithTraits(
        FROM_HERE, {BrowserThread::UI},
        base::BindOnce(&TabSpecificContentSettings::IndexedDBAccessed,
                       it.child_id, it.frame_routing_id, url, !allow));
  }

  return allow;
}
