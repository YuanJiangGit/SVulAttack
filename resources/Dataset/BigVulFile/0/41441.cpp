static int lookup_chan_dst(u16 call_id, __be32 d_addr)
{
	struct pppox_sock *sock;
	struct pptp_opt *opt;
	int i;

	rcu_read_lock();
	i = 1;
	for_each_set_bit_from(i, callid_bitmap, MAX_CALLID) {
		sock = rcu_dereference(callid_sock[i]);
		if (!sock)
			continue;
		opt = &sock->proto.pptp;
		if (opt->dst_addr.call_id == call_id &&
			  opt->dst_addr.sin_addr.s_addr == d_addr)
			break;
	}
	rcu_read_unlock();

	return i < MAX_CALLID;
}
