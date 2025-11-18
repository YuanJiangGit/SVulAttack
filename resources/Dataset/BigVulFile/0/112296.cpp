void ShellWindow::RequestMediaAccessPermission(
    content::WebContents* web_contents,
    const content::MediaStreamRequest* request,
    const content::MediaResponseCallback& callback) {
  content::MediaStreamDevices devices;

  content::MediaStreamDeviceMap::const_iterator iter =
      request->devices.find(content::MEDIA_STREAM_DEVICE_TYPE_AUDIO_CAPTURE);
  if (iter != request->devices.end() &&
      extension()->HasAPIPermission(ExtensionAPIPermission::kAudioCapture) &&
      !iter->second.empty()) {
    devices.push_back(iter->second[0]);
  }

  iter = request->devices.find(content::MEDIA_STREAM_DEVICE_TYPE_VIDEO_CAPTURE);
  if (iter != request->devices.end() &&
      extension()->HasAPIPermission(ExtensionAPIPermission::kVideoCapture) &&
      !iter->second.empty()) {
    devices.push_back(iter->second[0]);
  }

  callback.Run(devices);
}
