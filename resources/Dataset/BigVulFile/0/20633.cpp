static void kvm_set_mmio_data(struct kvm_vcpu *vcpu)
{
	struct kvm_mmio_req *p = kvm_get_vcpu_ioreq(vcpu);

	if (!vcpu->mmio_is_write)
		memcpy(&p->data, vcpu->mmio_data, 8);
	p->state = STATE_IORESP_READY;
}
