const KURL& Document::firstPartyForCookies() const
{
    if (SchemeRegistry::shouldTreatURLSchemeAsFirstPartyWhenTopLevel(topDocument().url().protocol()))
        return topDocument().url();

    OriginAccessEntry accessEntry(topDocument().url().protocol(), topDocument().url().host(), OriginAccessEntry::AllowRegisterableDomains);
    const Document* currentDocument = this;
    while (currentDocument) {
        while (currentDocument->isSrcdocDocument())
            currentDocument = currentDocument->parentDocument();
        ASSERT(currentDocument);

        if (accessEntry.matchesOrigin(*currentDocument->securityOrigin()) == OriginAccessEntry::DoesNotMatchOrigin)
            return SecurityOrigin::urlWithUniqueSecurityOrigin();

        currentDocument = currentDocument->parentDocument();
    }

    return topDocument().url();
}
