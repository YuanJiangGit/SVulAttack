static void perf_event_mmap_event(struct perf_mmap_event *mmap_event)
{
	struct vm_area_struct *vma = mmap_event->vma;
	struct file *file = vma->vm_file;
	int maj = 0, min = 0;
	u64 ino = 0, gen = 0;
	u32 prot = 0, flags = 0;
	unsigned int size;
	char tmp[16];
	char *buf = NULL;
	char *name;

	if (file) {
		struct inode *inode;
		dev_t dev;

		buf = kmalloc(PATH_MAX, GFP_KERNEL);
		if (!buf) {
			name = "//enomem";
			goto cpy_name;
		}
		/*
		 * d_path() works from the end of the rb backwards, so we
		 * need to add enough zero bytes after the string to handle
		 * the 64bit alignment we do later.
		 */
		name = d_path(&file->f_path, buf, PATH_MAX - sizeof(u64));
		if (IS_ERR(name)) {
			name = "//toolong";
			goto cpy_name;
		}
		inode = file_inode(vma->vm_file);
		dev = inode->i_sb->s_dev;
		ino = inode->i_ino;
		gen = inode->i_generation;
		maj = MAJOR(dev);
		min = MINOR(dev);

		if (vma->vm_flags & VM_READ)
			prot |= PROT_READ;
		if (vma->vm_flags & VM_WRITE)
			prot |= PROT_WRITE;
		if (vma->vm_flags & VM_EXEC)
			prot |= PROT_EXEC;

		if (vma->vm_flags & VM_MAYSHARE)
			flags = MAP_SHARED;
		else
			flags = MAP_PRIVATE;

		if (vma->vm_flags & VM_DENYWRITE)
			flags |= MAP_DENYWRITE;
		if (vma->vm_flags & VM_MAYEXEC)
			flags |= MAP_EXECUTABLE;
		if (vma->vm_flags & VM_LOCKED)
			flags |= MAP_LOCKED;
		if (vma->vm_flags & VM_HUGETLB)
			flags |= MAP_HUGETLB;

		goto got_name;
	} else {
		if (vma->vm_ops && vma->vm_ops->name) {
			name = (char *) vma->vm_ops->name(vma);
			if (name)
				goto cpy_name;
		}

		name = (char *)arch_vma_name(vma);
		if (name)
			goto cpy_name;

		if (vma->vm_start <= vma->vm_mm->start_brk &&
				vma->vm_end >= vma->vm_mm->brk) {
			name = "[heap]";
			goto cpy_name;
		}
		if (vma->vm_start <= vma->vm_mm->start_stack &&
				vma->vm_end >= vma->vm_mm->start_stack) {
			name = "[stack]";
			goto cpy_name;
		}

		name = "//anon";
		goto cpy_name;
	}

cpy_name:
	strlcpy(tmp, name, sizeof(tmp));
	name = tmp;
got_name:
	/*
	 * Since our buffer works in 8 byte units we need to align our string
	 * size to a multiple of 8. However, we must guarantee the tail end is
	 * zero'd out to avoid leaking random bits to userspace.
	 */
	size = strlen(name)+1;
	while (!IS_ALIGNED(size, sizeof(u64)))
		name[size++] = '\0';

	mmap_event->file_name = name;
	mmap_event->file_size = size;
	mmap_event->maj = maj;
	mmap_event->min = min;
	mmap_event->ino = ino;
	mmap_event->ino_generation = gen;
	mmap_event->prot = prot;
	mmap_event->flags = flags;

	if (!(vma->vm_flags & VM_EXEC))
		mmap_event->event_id.header.misc |= PERF_RECORD_MISC_MMAP_DATA;

	mmap_event->event_id.header.size = sizeof(mmap_event->event_id) + size;

	perf_event_aux(perf_event_mmap_output,
		       mmap_event,
		       NULL);

	kfree(buf);
}
