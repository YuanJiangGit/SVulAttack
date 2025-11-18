void InjectedBundlePage::beganEnterFullScreen(WKBundlePageRef, WKRect, WKRect)
{
    if (InjectedBundle::shared().testRunner()->shouldDumpFullScreenCallbacks())
        InjectedBundle::shared().outputText("beganEnterFullScreen()\n");
}
