bool ChromeContentRendererClient::IsAdblockInstalled() {
  return extension_dispatcher_->extensions()->Contains(
      "gighmmpiobklfepjocnamgkkbiglidom");
}
