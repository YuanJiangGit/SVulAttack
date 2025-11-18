static int __init vmx_init(void)
{
	int r, i, msr;

	rdmsrl_safe(MSR_EFER, &host_efer);

	for (i = 0; i < ARRAY_SIZE(vmx_msr_index); ++i)
		kvm_define_shared_msr(i, vmx_msr_index[i]);

	vmx_io_bitmap_a = (unsigned long *)__get_free_page(GFP_KERNEL);
	if (!vmx_io_bitmap_a)
		return -ENOMEM;

	r = -ENOMEM;

	vmx_io_bitmap_b = (unsigned long *)__get_free_page(GFP_KERNEL);
	if (!vmx_io_bitmap_b)
		goto out;

	vmx_msr_bitmap_legacy = (unsigned long *)__get_free_page(GFP_KERNEL);
	if (!vmx_msr_bitmap_legacy)
		goto out1;

	vmx_msr_bitmap_legacy_x2apic =
				(unsigned long *)__get_free_page(GFP_KERNEL);
	if (!vmx_msr_bitmap_legacy_x2apic)
		goto out2;

	vmx_msr_bitmap_longmode = (unsigned long *)__get_free_page(GFP_KERNEL);
	if (!vmx_msr_bitmap_longmode)
		goto out3;

	vmx_msr_bitmap_longmode_x2apic =
				(unsigned long *)__get_free_page(GFP_KERNEL);
	if (!vmx_msr_bitmap_longmode_x2apic)
		goto out4;
	vmx_vmread_bitmap = (unsigned long *)__get_free_page(GFP_KERNEL);
	if (!vmx_vmread_bitmap)
		goto out5;

	vmx_vmwrite_bitmap = (unsigned long *)__get_free_page(GFP_KERNEL);
	if (!vmx_vmwrite_bitmap)
		goto out6;

	memset(vmx_vmread_bitmap, 0xff, PAGE_SIZE);
	memset(vmx_vmwrite_bitmap, 0xff, PAGE_SIZE);

	/*
	 * Allow direct access to the PC debug port (it is often used for I/O
	 * delays, but the vmexits simply slow things down).
	 */
	memset(vmx_io_bitmap_a, 0xff, PAGE_SIZE);
	clear_bit(0x80, vmx_io_bitmap_a);

	memset(vmx_io_bitmap_b, 0xff, PAGE_SIZE);

	memset(vmx_msr_bitmap_legacy, 0xff, PAGE_SIZE);
	memset(vmx_msr_bitmap_longmode, 0xff, PAGE_SIZE);

	set_bit(0, vmx_vpid_bitmap); /* 0 is reserved for host */

	r = kvm_init(&vmx_x86_ops, sizeof(struct vcpu_vmx),
		     __alignof__(struct vcpu_vmx), THIS_MODULE);
	if (r)
		goto out7;

#ifdef CONFIG_KEXEC
	rcu_assign_pointer(crash_vmclear_loaded_vmcss,
			   crash_vmclear_local_loaded_vmcss);
#endif

	vmx_disable_intercept_for_msr(MSR_FS_BASE, false);
	vmx_disable_intercept_for_msr(MSR_GS_BASE, false);
	vmx_disable_intercept_for_msr(MSR_KERNEL_GS_BASE, true);
	vmx_disable_intercept_for_msr(MSR_IA32_SYSENTER_CS, false);
	vmx_disable_intercept_for_msr(MSR_IA32_SYSENTER_ESP, false);
	vmx_disable_intercept_for_msr(MSR_IA32_SYSENTER_EIP, false);
	vmx_disable_intercept_for_msr(MSR_IA32_BNDCFGS, true);

	memcpy(vmx_msr_bitmap_legacy_x2apic,
			vmx_msr_bitmap_legacy, PAGE_SIZE);
	memcpy(vmx_msr_bitmap_longmode_x2apic,
			vmx_msr_bitmap_longmode, PAGE_SIZE);

	if (enable_apicv) {
		for (msr = 0x800; msr <= 0x8ff; msr++)
			vmx_disable_intercept_msr_read_x2apic(msr);

		/* According SDM, in x2apic mode, the whole id reg is used.
		 * But in KVM, it only use the highest eight bits. Need to
		 * intercept it */
		vmx_enable_intercept_msr_read_x2apic(0x802);
		/* TMCCT */
		vmx_enable_intercept_msr_read_x2apic(0x839);
		/* TPR */
		vmx_disable_intercept_msr_write_x2apic(0x808);
		/* EOI */
		vmx_disable_intercept_msr_write_x2apic(0x80b);
		/* SELF-IPI */
		vmx_disable_intercept_msr_write_x2apic(0x83f);
	}

	if (enable_ept) {
		kvm_mmu_set_mask_ptes(0ull,
			(enable_ept_ad_bits) ? VMX_EPT_ACCESS_BIT : 0ull,
			(enable_ept_ad_bits) ? VMX_EPT_DIRTY_BIT : 0ull,
			0ull, VMX_EPT_EXECUTABLE_MASK);
		ept_set_mmio_spte_mask();
		kvm_enable_tdp();
	} else
		kvm_disable_tdp();

	update_ple_window_actual_max();

	return 0;

out7:
	free_page((unsigned long)vmx_vmwrite_bitmap);
out6:
	free_page((unsigned long)vmx_vmread_bitmap);
out5:
	free_page((unsigned long)vmx_msr_bitmap_longmode_x2apic);
out4:
	free_page((unsigned long)vmx_msr_bitmap_longmode);
out3:
	free_page((unsigned long)vmx_msr_bitmap_legacy_x2apic);
out2:
	free_page((unsigned long)vmx_msr_bitmap_legacy);
out1:
	free_page((unsigned long)vmx_io_bitmap_b);
out:
	free_page((unsigned long)vmx_io_bitmap_a);
	return r;
}
