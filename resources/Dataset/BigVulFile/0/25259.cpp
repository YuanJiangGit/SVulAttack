static inline int armv7_pmnc_counter_has_overflowed(unsigned long pmnc,
					enum armv7_counters counter)
{
	int ret = 0;

	if (counter == ARMV7_CYCLE_COUNTER)
		ret = pmnc & ARMV7_FLAG_C;
	else if ((counter >= ARMV7_COUNTER0) && (counter <= ARMV7_COUNTER_LAST))
		ret = pmnc & ARMV7_FLAG_P(counter);
	else
		pr_err("CPU%u checking wrong counter %d overflow status\n",
			smp_processor_id(), counter);

	return ret;
}
