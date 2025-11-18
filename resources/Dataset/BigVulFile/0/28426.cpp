struct dst_entry *fib6_rule_lookup(struct net *net, struct flowi6 *fl6,
				   int flags, pol_lookup_t lookup)
{
	return (struct dst_entry *) lookup(net, net->ipv6.fib6_main_tbl, fl6, flags);
}
