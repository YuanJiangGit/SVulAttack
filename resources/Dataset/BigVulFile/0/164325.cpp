bool TabsCaptureVisibleTabFunction::IsScreenshotEnabled() const {
  PrefService* service = chrome_details_.GetProfile()->GetPrefs();
  if (service->GetBoolean(prefs::kDisableScreenshots)) {
    return false;
  }
  return true;
}
