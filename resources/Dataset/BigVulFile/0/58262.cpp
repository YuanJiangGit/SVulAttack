static void __dma_clear_buffer(struct page *page, size_t size)
{
	/*
	 * Ensure that the allocated pages are zeroed, and that any data
	 * lurking in the kernel direct-mapped region is invalidated.
	 */
	if (PageHighMem(page)) {
		phys_addr_t base = __pfn_to_phys(page_to_pfn(page));
		phys_addr_t end = base + size;
		while (size > 0) {
			void *ptr = kmap_atomic(page);
			memset(ptr, 0, PAGE_SIZE);
			dmac_flush_range(ptr, ptr + PAGE_SIZE);
			kunmap_atomic(ptr);
			page++;
			size -= PAGE_SIZE;
		}
		outer_flush_range(base, end);
	} else {
		void *ptr = page_address(page);
		memset(ptr, 0, size);
		dmac_flush_range(ptr, ptr + size);
		outer_flush_range(__pa(ptr), __pa(ptr) + size);
	}
}
