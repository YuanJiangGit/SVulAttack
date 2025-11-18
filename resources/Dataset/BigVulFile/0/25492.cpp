static long set_intruction_bp(struct task_struct *child,
			      struct ppc_hw_breakpoint *bp_info)
{
	int slot;
	int slot1_in_use = ((child->thread.dbcr0 & DBCR0_IAC1) != 0);
	int slot2_in_use = ((child->thread.dbcr0 & DBCR0_IAC2) != 0);
	int slot3_in_use = ((child->thread.dbcr0 & DBCR0_IAC3) != 0);
	int slot4_in_use = ((child->thread.dbcr0 & DBCR0_IAC4) != 0);

	if (dbcr_iac_range(child) & DBCR_IAC12MODE)
		slot2_in_use = 1;
	if (dbcr_iac_range(child) & DBCR_IAC34MODE)
		slot4_in_use = 1;

	if (bp_info->addr >= TASK_SIZE)
		return -EIO;

	if (bp_info->addr_mode != PPC_BREAKPOINT_MODE_EXACT) {

		/* Make sure range is valid. */
		if (bp_info->addr2 >= TASK_SIZE)
			return -EIO;

		/* We need a pair of IAC regsisters */
		if ((!slot1_in_use) && (!slot2_in_use)) {
			slot = 1;
			child->thread.iac1 = bp_info->addr;
			child->thread.iac2 = bp_info->addr2;
			child->thread.dbcr0 |= DBCR0_IAC1;
			if (bp_info->addr_mode ==
					PPC_BREAKPOINT_MODE_RANGE_EXCLUSIVE)
				dbcr_iac_range(child) |= DBCR_IAC12X;
			else
				dbcr_iac_range(child) |= DBCR_IAC12I;
#if CONFIG_PPC_ADV_DEBUG_IACS > 2
		} else if ((!slot3_in_use) && (!slot4_in_use)) {
			slot = 3;
			child->thread.iac3 = bp_info->addr;
			child->thread.iac4 = bp_info->addr2;
			child->thread.dbcr0 |= DBCR0_IAC3;
			if (bp_info->addr_mode ==
					PPC_BREAKPOINT_MODE_RANGE_EXCLUSIVE)
				dbcr_iac_range(child) |= DBCR_IAC34X;
			else
				dbcr_iac_range(child) |= DBCR_IAC34I;
#endif
		} else
			return -ENOSPC;
	} else {
		/* We only need one.  If possible leave a pair free in
		 * case a range is needed later
		 */
		if (!slot1_in_use) {
			/*
			 * Don't use iac1 if iac1-iac2 are free and either
			 * iac3 or iac4 (but not both) are free
			 */
			if (slot2_in_use || (slot3_in_use == slot4_in_use)) {
				slot = 1;
				child->thread.iac1 = bp_info->addr;
				child->thread.dbcr0 |= DBCR0_IAC1;
				goto out;
			}
		}
		if (!slot2_in_use) {
			slot = 2;
			child->thread.iac2 = bp_info->addr;
			child->thread.dbcr0 |= DBCR0_IAC2;
#if CONFIG_PPC_ADV_DEBUG_IACS > 2
		} else if (!slot3_in_use) {
			slot = 3;
			child->thread.iac3 = bp_info->addr;
			child->thread.dbcr0 |= DBCR0_IAC3;
		} else if (!slot4_in_use) {
			slot = 4;
			child->thread.iac4 = bp_info->addr;
			child->thread.dbcr0 |= DBCR0_IAC4;
#endif
		} else
			return -ENOSPC;
	}
out:
	child->thread.dbcr0 |= DBCR0_IDM;
	child->thread.regs->msr |= MSR_DE;

	return slot;
}
