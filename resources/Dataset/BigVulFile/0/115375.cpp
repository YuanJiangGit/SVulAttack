void InjectedBundlePage::didCommitLoadForFrame(WKBundleFrameRef frame)
{
    if (!InjectedBundle::shared().isTestRunning())
        return;

    if (!InjectedBundle::shared().testRunner()->shouldDumpFrameLoadCallbacks())
        return;

    dumpLoadEvent(frame, "didCommitLoadForFrame");
}
