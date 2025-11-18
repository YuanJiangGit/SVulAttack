void ExtensionService::LoadExtension(const FilePath& extension_path) {
  if (!BrowserThread::PostTask(
          BrowserThread::FILE, FROM_HERE,
          NewRunnableMethod(
              backend_.get(),
              &ExtensionServiceBackend::LoadSingleExtension,
              extension_path)))
    NOTREACHED();
}
