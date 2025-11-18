static void mincore_hugetlb_page_range(struct vm_area_struct *vma,
				unsigned long addr, unsigned long end,
				unsigned char *vec)
{
#ifdef CONFIG_HUGETLB_PAGE
	struct hstate *h;

	h = hstate_vma(vma);
	while (1) {
		unsigned char present;
		pte_t *ptep;
		/*
		 * Huge pages are always in RAM for now, but
		 * theoretically it needs to be checked.
		 */
		ptep = huge_pte_offset(current->mm,
				       addr & huge_page_mask(h));
		present = ptep && !huge_pte_none(huge_ptep_get(ptep));
		while (1) {
			*vec = present;
			vec++;
			addr += PAGE_SIZE;
			if (addr == end)
				return;
			/* check hugepage border */
			if (!(addr & ~huge_page_mask(h)))
				break;
		}
	}
#else
	BUG();
#endif
}
