void Document::partialUpdateLayoutIgnorePendingStylesheets(Node* stopLayoutAtNode)
{
    if (!RuntimeEnabledFeatures::partialLayoutEnabled() || !ScrollbarTheme::theme()->usesOverlayScrollbars()) {
        updateLayoutIgnorePendingStylesheets();
        return;
    }

    TemporaryChange<bool> ignorePendingStylesheets(m_ignorePendingStylesheets, m_ignorePendingStylesheets);
    recalcStyleForLayoutIgnoringPendingStylesheets();

    if (stopLayoutAtNode) {
        RenderObject* renderer = stopLayoutAtNode->renderer();
        bool canPartialLayout = renderer;
        while (renderer) {
            if (!renderer->supportsPartialLayout()) {
                canPartialLayout = false;
                break;
            }
            renderer = renderer->parent();
        }
        if (canPartialLayout && view())
            view()->partialLayout().setStopAtRenderer(stopLayoutAtNode->renderer());
    }

    updateLayout();

    if (view())
        view()->partialLayout().reset();
}
