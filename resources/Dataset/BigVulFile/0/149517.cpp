void LoadingDataCollector::RecordFinishNavigation(
    const NavigationID& old_navigation_id,
    const NavigationID& new_navigation_id,
    bool is_error_page) {
  if (is_error_page) {
    inflight_navigations_.erase(old_navigation_id);
    return;
  }

  std::unique_ptr<PageRequestSummary> summary;
  auto nav_it = inflight_navigations_.find(old_navigation_id);
  if (nav_it != inflight_navigations_.end()) {
    summary = std::move(nav_it->second);
    DCHECK_EQ(summary->main_frame_url, old_navigation_id.main_frame_url);
    summary->main_frame_url = new_navigation_id.main_frame_url;
    inflight_navigations_.erase(nav_it);
  } else {
    summary =
        std::make_unique<PageRequestSummary>(new_navigation_id.main_frame_url);
    summary->initial_url = old_navigation_id.main_frame_url;
  }

  inflight_navigations_.emplace(new_navigation_id, std::move(summary));
}
