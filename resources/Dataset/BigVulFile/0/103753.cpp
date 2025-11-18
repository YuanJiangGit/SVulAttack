bool RenderThread::IsRegisteredExtension(
    const std::string& v8_extension_name) const {
  return v8_extensions_.find(v8_extension_name) != v8_extensions_.end();
}
