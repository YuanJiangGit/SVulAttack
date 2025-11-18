void RenderThread::EnsureWebKitInitialized() {
  if (webkit_client_.get())
    return;

  v8::V8::SetCounterFunction(base::StatsTable::FindLocation);
  v8::V8::SetCreateHistogramFunction(CreateHistogram);
  v8::V8::SetAddHistogramSampleFunction(AddHistogramSample);

  webkit_client_.reset(new RendererWebKitClientImpl);
  WebKit::initialize(webkit_client_.get());

  WebScriptController::enableV8SingleThreadMode();

  const CommandLine& command_line = *CommandLine::ForCurrentProcess();

  webkit_glue::EnableWebCoreLogChannels(
      command_line.GetSwitchValueASCII(switches::kWebCoreLogChannels));

  if (command_line.HasSwitch(switches::kEnableBenchmarking))
    RegisterExtension(extensions_v8::BenchmarkingExtension::Get());

  if (command_line.HasSwitch(switches::kPlaybackMode) ||
      command_line.HasSwitch(switches::kRecordMode) ||
      command_line.HasSwitch(switches::kNoJsRandomness)) {
    RegisterExtension(extensions_v8::PlaybackExtension::Get());
  }

  web_database_observer_impl_.reset(new WebDatabaseObserverImpl(this));
  WebKit::WebDatabase::setObserver(web_database_observer_impl_.get());

  WebRuntimeFeatures::enableSockets(
      !command_line.HasSwitch(switches::kDisableWebSockets));

  WebRuntimeFeatures::enableDatabase(
      !command_line.HasSwitch(switches::kDisableDatabases));

  WebRuntimeFeatures::enableDataTransferItems(
      !command_line.HasSwitch(switches::kDisableDataTransferItems));

  WebRuntimeFeatures::enableApplicationCache(
      !command_line.HasSwitch(switches::kDisableApplicationCache));

  WebRuntimeFeatures::enableNotifications(
      !command_line.HasSwitch(switches::kDisableDesktopNotifications));

  WebRuntimeFeatures::enableLocalStorage(
      !command_line.HasSwitch(switches::kDisableLocalStorage));
  WebRuntimeFeatures::enableSessionStorage(
      !command_line.HasSwitch(switches::kDisableSessionStorage));

  WebRuntimeFeatures::enableIndexedDatabase(
      !command_line.HasSwitch(switches::kDisableIndexedDatabase));

  WebRuntimeFeatures::enableGeolocation(
      !command_line.HasSwitch(switches::kDisableGeolocation));

  WebKit::WebRuntimeFeatures::enableMediaStream(
      command_line.HasSwitch(switches::kEnableMediaStream));

#if defined(OS_CHROMEOS)
  WebRuntimeFeatures::enableWebAudio(false);
#else
  WebRuntimeFeatures::enableWebAudio(
      !command_line.HasSwitch(switches::kDisableWebAudio));
#endif

  WebRuntimeFeatures::enablePushState(true);

#ifdef TOUCH_UI
  WebRuntimeFeatures::enableTouch(true);
#else
  WebRuntimeFeatures::enableTouch(false);
#endif

  WebRuntimeFeatures::enableDeviceMotion(
      command_line.HasSwitch(switches::kEnableDeviceMotion));

  WebRuntimeFeatures::enableDeviceOrientation(
      !command_line.HasSwitch(switches::kDisableDeviceOrientation));

  WebRuntimeFeatures::enableSpeechInput(
      !command_line.HasSwitch(switches::kDisableSpeechInput));

  WebRuntimeFeatures::enableFileSystem(
      !command_line.HasSwitch(switches::kDisableFileSystem));

  WebRuntimeFeatures::enableJavaScriptI18NAPI(
      !command_line.HasSwitch(switches::kDisableJavaScriptI18NAPI));

  WebRuntimeFeatures::enableQuota(true);

  FOR_EACH_OBSERVER(RenderProcessObserver, observers_, WebKitInitialized());
}
