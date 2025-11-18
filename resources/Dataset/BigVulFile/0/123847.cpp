WebKit::WebPlugin* RenderViewImpl::CreatePluginReplacement(
    const base::FilePath& file_path) {
  return GetContentClient()->renderer()->CreatePluginReplacement(
      this, file_path);
}
