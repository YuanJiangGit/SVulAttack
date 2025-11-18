void TabSpecificContentSettings::CookieChanged(
    int render_process_id,
    int render_view_id,
    const GURL& url,
    const GURL& frame_url,
    const std::string& cookie_line,
    const net::CookieOptions& options,
    bool blocked_by_policy) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  TabSpecificContentSettings* settings = Get(render_process_id, render_view_id);
  if (settings)
    settings->OnCookieChanged(url, frame_url, cookie_line, options,
                              blocked_by_policy);
}
