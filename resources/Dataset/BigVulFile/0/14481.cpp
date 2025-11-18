static inline uint16_t vring_avail_flags(VirtQueue *vq)
{
    hwaddr pa;
    pa = vq->vring.avail + offsetof(VRingAvail, flags);
    return lduw_phys(&address_space_memory, pa);
}
