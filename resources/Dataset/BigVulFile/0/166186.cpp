  void OnStreamGenerated(int request_id,
                         MediaStreamRequestResult result,
                         const std::string& label,
                         const MediaStreamDevices& audio_devices,
                         const MediaStreamDevices& video_devices) {
    if (result != MEDIA_DEVICE_OK) {
      OnStreamGenerationFailed(request_id, result);
      return;
    }

    OnStreamGenerationSuccess(request_id, audio_devices.size(),
                              video_devices.size());
    OnStreamStarted(label);

    base::Closure quit_closure = quit_closures_.front();
    quit_closures_.pop();
    task_runner_->PostTask(FROM_HERE, base::ResetAndReturn(&quit_closure));

    label_ = label;
    audio_devices_ = audio_devices;
    video_devices_ = video_devices;
  }
