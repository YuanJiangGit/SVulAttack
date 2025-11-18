const Extension* ExtensionSettingsHandler::GetExtension(const ListValue* args) {
  std::string extension_id = UTF16ToUTF8(ExtractStringValue(args));
  CHECK(!extension_id.empty());
  return extension_service_->GetExtensionById(extension_id, true);
}
