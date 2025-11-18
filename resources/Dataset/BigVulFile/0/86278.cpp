static struct net_generic *net_alloc_generic(void)
{
	struct net_generic *ng;
	unsigned int generic_size = offsetof(struct net_generic, ptr[max_gen_ptrs]);

	ng = kzalloc(generic_size, GFP_KERNEL);
	if (ng)
		ng->s.len = max_gen_ptrs;

	return ng;
}
