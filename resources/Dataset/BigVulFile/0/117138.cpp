void webkitWebViewBaseForwardNextKeyEvent(WebKitWebViewBase* webkitWebViewBase)
{
    webkitWebViewBase->priv->shouldForwardNextKeyEvent = TRUE;
}
