size_t DisplayItemList::findMatchingItemFromIndex(const DisplayItem::Id& id, const DisplayItemIndicesByClientMap& displayItemIndicesByClient, const DisplayItems& list)
{
    DisplayItemIndicesByClientMap::const_iterator it = displayItemIndicesByClient.find(id.client);
    if (it == displayItemIndicesByClient.end())
        return kNotFound;

    const Vector<size_t>& indices = it->value;
    for (size_t index : indices) {
        const DisplayItem& existingItem = list[index];
        ASSERT(!existingItem.isValid() || existingItem.client() == id.client);
        if (existingItem.isValid() && id.matches(existingItem))
            return index;
    }

    return kNotFound;
}
