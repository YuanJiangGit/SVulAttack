void OfflinePageModelImpl::OnRemoveOfflinePagesDone(
    const DeletePageCallback& callback,
    std::unique_ptr<OfflinePagesUpdateResult> result) {
  ReportPageHistogramsAfterDelete(offline_pages_, result->updated_items,
                                  GetCurrentTime());

  for (const auto& page : result->updated_items) {
    int64_t offline_id = page.offline_id;
    offline_event_logger_.RecordPageDeleted(offline_id);
    auto iter = offline_pages_.find(offline_id);
    if (iter == offline_pages_.end())
      continue;
    offline_pages_.erase(iter);
  }

  for (const auto& page : result->updated_items) {
    DeletedPageInfo info(page.offline_id, page.client_id, page.request_origin);
    for (Observer& observer : observers_)
      observer.OfflinePageDeleted(info);
  }

  DeletePageResult delete_result;
  if (result->store_state == StoreState::LOADED)
    delete_result = DeletePageResult::SUCCESS;
  else
    delete_result = DeletePageResult::STORE_FAILURE;

  InformDeletePageDone(callback, delete_result);
}
