bool HeapAllocator::expandHashTableBacking(void* address, size_t newSize) {
  return backingExpand(address, newSize);
}
