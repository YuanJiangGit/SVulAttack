int cifs_file_mmap(struct file *file, struct vm_area_struct *vma)
{
	int rc, xid;

	xid = get_xid();
	rc = cifs_revalidate_file(file);
	if (rc) {
		cifs_dbg(FYI, "Validation prior to mmap failed, error=%d\n",
			 rc);
		free_xid(xid);
		return rc;
	}
	rc = generic_file_mmap(file, vma);
	if (rc == 0)
		vma->vm_ops = &cifs_file_vm_ops;
	free_xid(xid);
	return rc;
}
