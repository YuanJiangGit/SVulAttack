void Browser::OnPreferenceChanged(PrefServiceBase* service,
                                  const std::string& pref_name) {
  if (pref_name == prefs::kDevToolsDisabled) {
    if (profile_->GetPrefs()->GetBoolean(prefs::kDevToolsDisabled))
      content::DevToolsManager::GetInstance()->CloseAllClientHosts();
  } else if (pref_name == prefs::kShowBookmarkBar) {
    UpdateBookmarkBarState(BOOKMARK_BAR_STATE_CHANGE_PREF_CHANGE);
  } else if (pref_name == prefs::kHomePage) {
    MarkHomePageAsChanged(static_cast<PrefService*>(service));
  } else {
    NOTREACHED();
  }
}
