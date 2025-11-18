PassRefPtrWillBeRawPtr<HTMLCollection> Document::links()
{
    return ensureCachedCollection<HTMLCollection>(DocLinks);
}
