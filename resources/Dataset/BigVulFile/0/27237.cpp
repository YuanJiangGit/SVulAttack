arch_get_unmapped_area_1(unsigned long addr, unsigned long len,
		         unsigned long limit)
{
	struct vm_area_struct *vma = find_vma(current->mm, addr);

	while (1) {
		/* At this point:  (!vma || addr < vma->vm_end). */
		if (limit - len < addr)
			return -ENOMEM;
		if (!vma || addr + len <= vma->vm_start)
			return addr;
		addr = vma->vm_end;
		vma = vma->vm_next;
	}
}
