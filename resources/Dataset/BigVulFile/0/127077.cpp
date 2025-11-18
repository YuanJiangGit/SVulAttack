void ChromeClientImpl::setTouchAction(TouchAction touchAction)
{
    if (WebViewClient* client = m_webView->client()) {
        WebTouchAction webTouchAction = static_cast<WebTouchAction>(touchAction);
        client->setTouchAction(webTouchAction);
    }
}
