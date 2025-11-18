static void *established_get_first(struct seq_file *seq)
{
	struct tcp_iter_state *st = seq->private;
	struct net *net = seq_file_net(seq);
	void *rc = NULL;

	st->offset = 0;
	for (; st->bucket <= tcp_hashinfo.ehash_mask; ++st->bucket) {
		struct sock *sk;
		struct hlist_nulls_node *node;
		struct inet_timewait_sock *tw;
		spinlock_t *lock = inet_ehash_lockp(&tcp_hashinfo, st->bucket);

		/* Lockless fast path for the common case of empty buckets */
		if (empty_bucket(st))
			continue;

		spin_lock_bh(lock);
		sk_nulls_for_each(sk, node, &tcp_hashinfo.ehash[st->bucket].chain) {
			if (sk->sk_family != st->family ||
			    !net_eq(sock_net(sk), net)) {
				continue;
			}
			rc = sk;
			goto out;
		}
		st->state = TCP_SEQ_STATE_TIME_WAIT;
		inet_twsk_for_each(tw, node,
				   &tcp_hashinfo.ehash[st->bucket].twchain) {
			if (tw->tw_family != st->family ||
			    !net_eq(twsk_net(tw), net)) {
				continue;
			}
			rc = tw;
			goto out;
		}
		spin_unlock_bh(lock);
		st->state = TCP_SEQ_STATE_ESTABLISHED;
	}
out:
	return rc;
}
