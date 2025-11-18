static bool compare_ptrs_to_packet(struct bpf_reg_state *old,
				   struct bpf_reg_state *cur)
{
	if (old->id != cur->id)
		return false;

	/* old ptr_to_packet is more conservative, since it allows smaller
	 * range. Ex:
	 * old(off=0,r=10) is equal to cur(off=0,r=20), because
	 * old(off=0,r=10) means that with range=10 the verifier proceeded
	 * further and found no issues with the program. Now we're in the same
	 * spot with cur(off=0,r=20), so we're safe too, since anything further
	 * will only be looking at most 10 bytes after this pointer.
	 */
	if (old->off == cur->off && old->range < cur->range)
		return true;

	/* old(off=20,r=10) is equal to cur(off=22,re=22 or 5 or 0)
	 * since both cannot be used for packet access and safe(old)
	 * pointer has smaller off that could be used for further
	 * 'if (ptr > data_end)' check
	 * Ex:
	 * old(off=20,r=10) and cur(off=22,r=22) and cur(off=22,r=0) mean
	 * that we cannot access the packet.
	 * The safe range is:
	 * [ptr, ptr + range - off)
	 * so whenever off >=range, it means no safe bytes from this pointer.
	 * When comparing old->off <= cur->off, it means that older code
	 * went with smaller offset and that offset was later
	 * used to figure out the safe range after 'if (ptr > data_end)' check
	 * Say, 'old' state was explored like:
	 * ... R3(off=0, r=0)
	 * R4 = R3 + 20
	 * ... now R4(off=20,r=0)  <-- here
	 * if (R4 > data_end)
	 * ... R4(off=20,r=20), R3(off=0,r=20) and R3 can be used to access.
	 * ... the code further went all the way to bpf_exit.
	 * Now the 'cur' state at the mark 'here' has R4(off=30,r=0).
	 * old_R4(off=20,r=0) equal to cur_R4(off=30,r=0), since if the verifier
	 * goes further, such cur_R4 will give larger safe packet range after
	 * 'if (R4 > data_end)' and all further insn were already good with r=20,
	 * so they will be good with r=30 and we can prune the search.
	 */
	if (old->off <= cur->off &&
	    old->off >= old->range && cur->off >= cur->range)
		return true;

	return false;
}
