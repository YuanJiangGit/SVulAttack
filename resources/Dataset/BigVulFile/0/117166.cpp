    static gboolean bringToFrontCallback(WebKitWebInspector*, InspectorTest* test)
    {
        return test->bringToFront();
    }
