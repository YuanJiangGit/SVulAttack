static int vm_stat_get(void *_offset, u64 *val)
{
	unsigned offset = (long)_offset;
	struct kvm *kvm;

	*val = 0;
	spin_lock(&kvm_lock);
	list_for_each_entry(kvm, &vm_list, vm_list)
		*val += *(u32 *)((void *)kvm + offset);
	spin_unlock(&kvm_lock);
	return 0;
}
