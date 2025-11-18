void DownloadItemImpl::SetDangerType(content::DownloadDangerType danger_type) {
  danger_type_ = danger_type;
  SafetyState updated_value = IsDangerous() ?
      DownloadItem::DANGEROUS : DownloadItem::SAFE;
  if (updated_value != safety_state_) {
    safety_state_ = updated_value;
    bound_net_log_.AddEvent(
        net::NetLog::TYPE_DOWNLOAD_ITEM_SAFETY_STATE_UPDATED,
        base::Bind(&download_net_logs::ItemCheckedCallback,
                   GetDangerType(), GetSafetyState()));
    UpdateObservers();
  }
}
