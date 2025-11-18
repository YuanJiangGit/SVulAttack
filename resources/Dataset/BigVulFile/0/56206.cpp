static inline int armv8pmu_enable_counter(int idx)
{
	u32 counter;

	if (!armv8pmu_counter_valid(idx)) {
		pr_err("CPU%u enabling wrong PMNC counter %d\n",
			smp_processor_id(), idx);
		return -EINVAL;
	}

	counter = ARMV8_IDX_TO_COUNTER(idx);
	asm volatile("msr pmcntenset_el0, %0" :: "r" (BIT(counter)));
	return idx;
}
