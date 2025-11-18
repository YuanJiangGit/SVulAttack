void Document::RegisterNodeListWithIdNameCache(const LiveNodeListBase* list) {
  DCHECK(!node_lists_[kInvalidateOnIdNameAttrChange].Contains(list));
  node_lists_[kInvalidateOnIdNameAttrChange].insert(list);
  LiveNodeListBaseWriteBarrier(this, list);
}
