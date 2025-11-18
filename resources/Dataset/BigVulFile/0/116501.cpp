bool AttachDebuggerFunction::RunImpl() {
  if (!InitTabContents())
    return false;

  std::string version;
  EXTENSION_FUNCTION_VALIDATE(args_->GetString(1, &version));

  if (!webkit_glue::IsInspectorProtocolVersionSupported(version)) {
    error_ = ExtensionErrorUtils::FormatErrorMessage(
        keys::kProtocolVersionNotSupportedError,
        version);
    return false;
  }

  DevToolsAgentHost* agent = DevToolsAgentHostRegistry::GetDevToolsAgentHost(
      contents_->GetRenderViewHost());
  DevToolsClientHost* client_host = DevToolsManager::GetInstance()->
      GetDevToolsClientHostFor(agent);

  if (client_host != NULL) {
    error_ = ExtensionErrorUtils::FormatErrorMessage(
        keys::kAlreadyAttachedError,
        base::IntToString(tab_id_));
    return false;
  }

  new ExtensionDevToolsClientHost(contents_,
                                  GetExtension()->id(),
                                  GetExtension()->name(),
                                  tab_id_);
  SendResponse(true);
  return true;
}
