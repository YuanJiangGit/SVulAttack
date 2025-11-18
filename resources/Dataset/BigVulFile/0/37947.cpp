int kvm_iommu_map_guest(struct kvm *kvm)
{
	int r;

	if (!iommu_present(&pci_bus_type)) {
		printk(KERN_ERR "%s: iommu not found\n", __func__);
		return -ENODEV;
	}

	mutex_lock(&kvm->slots_lock);

	kvm->arch.iommu_domain = iommu_domain_alloc(&pci_bus_type);
	if (!kvm->arch.iommu_domain) {
		r = -ENOMEM;
		goto out_unlock;
	}

	if (!allow_unsafe_assigned_interrupts &&
	    !iommu_domain_has_cap(kvm->arch.iommu_domain,
				  IOMMU_CAP_INTR_REMAP)) {
		printk(KERN_WARNING "%s: No interrupt remapping support,"
		       " disallowing device assignment."
		       " Re-enble with \"allow_unsafe_assigned_interrupts=1\""
		       " module option.\n", __func__);
		iommu_domain_free(kvm->arch.iommu_domain);
		kvm->arch.iommu_domain = NULL;
		r = -EPERM;
		goto out_unlock;
	}

	r = kvm_iommu_map_memslots(kvm);
	if (r)
		kvm_iommu_unmap_memslots(kvm);

out_unlock:
	mutex_unlock(&kvm->slots_lock);
 	return r;
 }
