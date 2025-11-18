void InspectorPageAgent::getCookies(ErrorString*, RefPtr<TypeBuilder::Array<TypeBuilder::Page::Cookie> >& cookies, WTF::String* cookiesString)
{
    ListHashSet<Cookie> rawCookiesList;

    for (Frame* frame = mainFrame(); frame; frame = frame->tree().traverseNext(mainFrame())) {
        Document* document = frame->document();
        Vector<KURL> allURLs = allResourcesURLsForFrame(frame);
        for (Vector<KURL>::const_iterator it = allURLs.begin(); it != allURLs.end(); ++it) {
            Vector<Cookie> docCookiesList;
            getRawCookies(document, *it, docCookiesList);
            int cookiesSize = docCookiesList.size();
            for (int i = 0; i < cookiesSize; i++) {
                if (!rawCookiesList.contains(docCookiesList[i]))
                    rawCookiesList.add(docCookiesList[i]);
            }
        }
    }

    cookies = buildArrayForCookies(rawCookiesList);
    *cookiesString = "";
}
