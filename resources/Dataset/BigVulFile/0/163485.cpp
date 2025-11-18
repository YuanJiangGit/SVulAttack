void PushMessagingServiceImpl::DidDeleteID(const std::string& app_id,
                                           bool was_subscribed,
                                           InstanceID::Result result) {
  RecordUnsubscribeIIDResult(result);
  base::ThreadTaskRunnerHandle::Get()->PostTask(
      FROM_HERE,
      base::BindOnce(&PushMessagingServiceImpl::DidUnsubscribe,
                     weak_factory_.GetWeakPtr(), app_id, was_subscribed));
}
