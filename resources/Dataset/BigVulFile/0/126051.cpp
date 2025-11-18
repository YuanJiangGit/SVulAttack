void GetAllNotificationsObserver::SendMessage() {
  NotificationUIManager* manager =
      g_browser_process->notification_ui_manager();
  const BalloonCollection::Balloons& balloons =
      manager->balloon_collection()->GetActiveBalloons();
  DictionaryValue return_value;
  ListValue* list = new ListValue;
  return_value.Set("notifications", list);
  BalloonCollection::Balloons::const_iterator balloon_iter;
  for (balloon_iter = balloons.begin(); balloon_iter != balloons.end();
       ++balloon_iter) {
    base::DictionaryValue* note = NotificationToJson(
        &(*balloon_iter)->notification());
    BalloonHost* balloon_host = (*balloon_iter)->balloon_view()->GetHost();
    if (balloon_host) {
      int pid = base::GetProcId(balloon_host->web_contents()->
                                GetRenderViewHost()->GetProcess()->GetHandle());
      note->SetInteger("pid", pid);
    }
    list->Append(note);
  }
  std::vector<const Notification*> queued_notes;
  manager->GetQueuedNotificationsForTesting(&queued_notes);
  std::vector<const Notification*>::const_iterator queued_iter;
  for (queued_iter = queued_notes.begin(); queued_iter != queued_notes.end();
       ++queued_iter) {
    list->Append(NotificationToJson(*queued_iter));
  }
  AutomationJSONReply(automation_,
                      reply_message_.release()).SendSuccess(&return_value);
  delete this;
}
