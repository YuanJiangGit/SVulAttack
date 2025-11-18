PassRefPtr<HTMLCollection> Document::ensureCachedCollection(CollectionType type)
{
    return ensureRareData()->ensureNodeLists()->addCacheWithAtomicName<HTMLCollection>(this, type);
}
