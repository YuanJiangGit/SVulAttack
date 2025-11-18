ContentSettingsClient* FrameFetchContext::GetContentSettingsClient() const {
  if (IsDetached())
    return nullptr;
  return GetFrame()->GetContentSettingsClient();
}
