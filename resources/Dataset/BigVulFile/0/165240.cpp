bool ArcVoiceInteractionFrameworkService::ValidateTimeSinceUserInteraction() {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);

  if (!context_request_remaining_count_) {
    LOG(ERROR) << "Illegal context request from container.";
    UMA_HISTOGRAM_BOOLEAN("VoiceInteraction.IllegalContextRequest", true);
    return false;
  }
  auto elapsed = base::TimeTicks::Now() - user_interaction_start_time_;
  elapsed = elapsed > kMaxTimeSinceUserInteractionForHistogram
                ? kMaxTimeSinceUserInteractionForHistogram
                : elapsed;

  UMA_HISTOGRAM_CUSTOM_COUNTS(
      "VoiceInteraction.UserInteractionToRequestArrival",
      elapsed.InMilliseconds(), 1,
      kMaxTimeSinceUserInteractionForHistogram.InMilliseconds(), 20);

  if (elapsed > kAllowedTimeSinceUserInteraction) {
    LOG(ERROR) << "Timed out since last user interaction.";
    context_request_remaining_count_ = 0;
    return false;
  }

  context_request_remaining_count_--;
  return true;
}
