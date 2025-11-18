void ChromeRenderMessageFilter::OnOpenChannelToTab(
    int routing_id, int tab_id, const std::string& extension_id,
    const std::string& channel_name, int* port_id) {
  int port2_id;
  ExtensionMessageService::AllocatePortIdPair(port_id, &port2_id);

  BrowserThread::PostTask(
      BrowserThread::UI, FROM_HERE,
      base::Bind(&ChromeRenderMessageFilter::OpenChannelToTabOnUIThread, this,
                 render_process_id_, routing_id, port2_id, tab_id, extension_id,
                 channel_name));
}
