static void thp_pmd_to_pagemap_entry(pagemap_entry_t *pme, struct pagemapread *pm,
		pmd_t pmd, int offset, int pmd_flags2)
{
	/*
	 * Currently pmd for thp is always present because thp can not be
	 * swapped-out, migrated, or HWPOISONed (split in such cases instead.)
	 * This if-check is just to prepare for future implementation.
	 */
	if (pmd_present(pmd))
		*pme = make_pme(PM_PFRAME(pmd_pfn(pmd) + offset)
				| PM_STATUS2(pm->v2, pmd_flags2) | PM_PRESENT);
	else
		*pme = make_pme(PM_NOT_PRESENT(pm->v2) | PM_STATUS2(pm->v2, pmd_flags2));
}
