bool ExtensionApiTest::RunExtensionSubtestWithArgAndFlags(
    const std::string& extension_name,
    const std::string& page_url,
    const char* custom_arg,
    int flags) {
  DCHECK(!page_url.empty()) << "Argument page_url is required.";
  return RunExtensionTestImpl(extension_name, page_url, custom_arg, flags);
}
