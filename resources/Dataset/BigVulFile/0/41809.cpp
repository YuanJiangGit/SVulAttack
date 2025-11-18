static inline u32 cstamp_delta(unsigned long cstamp)
{
	return (cstamp - INITIAL_JIFFIES) * 100UL / HZ;
}
