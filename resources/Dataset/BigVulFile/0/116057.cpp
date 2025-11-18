std::vector<ExtensionPage> ExtensionSettingsHandler::GetActivePagesForExtension(
    const Extension* extension) {
  std::vector<ExtensionPage> result;

  ExtensionProcessManager* process_manager =
      extension_service_->profile()->GetExtensionProcessManager();
  GetActivePagesForExtensionProcess(
      process_manager->GetRenderViewHostsForExtension(
          extension->id()), &result);

  if (extension_service_->profile()->HasOffTheRecordProfile() &&
      extension->incognito_split_mode()) {
    ExtensionProcessManager* process_manager =
        extension_service_->profile()->GetOffTheRecordProfile()->
            GetExtensionProcessManager();
    GetActivePagesForExtensionProcess(
        process_manager->GetRenderViewHostsForExtension(
            extension->id()), &result);
  }

  return result;
}
