int hugetlb_fault(struct mm_struct *mm, struct vm_area_struct *vma,
			unsigned long address, unsigned int flags)
{
	pte_t *ptep, entry;
	spinlock_t *ptl;
	int ret;
	u32 hash;
	pgoff_t idx;
	struct page *page = NULL;
	struct page *pagecache_page = NULL;
	struct hstate *h = hstate_vma(vma);
	struct address_space *mapping;
	int need_wait_lock = 0;

	address &= huge_page_mask(h);

	ptep = huge_pte_offset(mm, address, huge_page_size(h));
	if (ptep) {
		entry = huge_ptep_get(ptep);
		if (unlikely(is_hugetlb_entry_migration(entry))) {
			migration_entry_wait_huge(vma, mm, ptep);
			return 0;
		} else if (unlikely(is_hugetlb_entry_hwpoisoned(entry)))
			return VM_FAULT_HWPOISON_LARGE |
				VM_FAULT_SET_HINDEX(hstate_index(h));
	} else {
		ptep = huge_pte_alloc(mm, address, huge_page_size(h));
		if (!ptep)
			return VM_FAULT_OOM;
	}

	mapping = vma->vm_file->f_mapping;
	idx = vma_hugecache_offset(h, vma, address);

	/*
	 * Serialize hugepage allocation and instantiation, so that we don't
	 * get spurious allocation failures if two CPUs race to instantiate
	 * the same page in the page cache.
	 */
	hash = hugetlb_fault_mutex_hash(h, mm, vma, mapping, idx, address);
	mutex_lock(&hugetlb_fault_mutex_table[hash]);

	entry = huge_ptep_get(ptep);
	if (huge_pte_none(entry)) {
		ret = hugetlb_no_page(mm, vma, mapping, idx, address, ptep, flags);
		goto out_mutex;
	}

	ret = 0;

	/*
	 * entry could be a migration/hwpoison entry at this point, so this
	 * check prevents the kernel from going below assuming that we have
	 * a active hugepage in pagecache. This goto expects the 2nd page fault,
	 * and is_hugetlb_entry_(migration|hwpoisoned) check will properly
	 * handle it.
	 */
	if (!pte_present(entry))
		goto out_mutex;

	/*
	 * If we are going to COW the mapping later, we examine the pending
	 * reservations for this page now. This will ensure that any
	 * allocations necessary to record that reservation occur outside the
	 * spinlock. For private mappings, we also lookup the pagecache
	 * page now as it is used to determine if a reservation has been
	 * consumed.
	 */
	if ((flags & FAULT_FLAG_WRITE) && !huge_pte_write(entry)) {
		if (vma_needs_reservation(h, vma, address) < 0) {
			ret = VM_FAULT_OOM;
			goto out_mutex;
		}
		/* Just decrements count, does not deallocate */
		vma_end_reservation(h, vma, address);

		if (!(vma->vm_flags & VM_MAYSHARE))
			pagecache_page = hugetlbfs_pagecache_page(h,
								vma, address);
	}

	ptl = huge_pte_lock(h, mm, ptep);

	/* Check for a racing update before calling hugetlb_cow */
	if (unlikely(!pte_same(entry, huge_ptep_get(ptep))))
		goto out_ptl;

	/*
	 * hugetlb_cow() requires page locks of pte_page(entry) and
	 * pagecache_page, so here we need take the former one
	 * when page != pagecache_page or !pagecache_page.
	 */
	page = pte_page(entry);
	if (page != pagecache_page)
		if (!trylock_page(page)) {
			need_wait_lock = 1;
			goto out_ptl;
		}

	get_page(page);

	if (flags & FAULT_FLAG_WRITE) {
		if (!huge_pte_write(entry)) {
			ret = hugetlb_cow(mm, vma, address, ptep,
					  pagecache_page, ptl);
			goto out_put_page;
		}
		entry = huge_pte_mkdirty(entry);
	}
	entry = pte_mkyoung(entry);
	if (huge_ptep_set_access_flags(vma, address, ptep, entry,
						flags & FAULT_FLAG_WRITE))
		update_mmu_cache(vma, address, ptep);
out_put_page:
	if (page != pagecache_page)
		unlock_page(page);
	put_page(page);
out_ptl:
	spin_unlock(ptl);

	if (pagecache_page) {
		unlock_page(pagecache_page);
		put_page(pagecache_page);
	}
out_mutex:
	mutex_unlock(&hugetlb_fault_mutex_table[hash]);
	/*
	 * Generally it's safe to hold refcount during waiting page lock. But
	 * here we just wait to defer the next page fault to avoid busy loop and
	 * the page is not used after unlocked before returning from the current
	 * page fault. So we are safe from accessing freed page, even if we wait
	 * here without taking refcount.
	 */
	if (need_wait_lock)
		wait_on_page_locked(page);
	return ret;
}
