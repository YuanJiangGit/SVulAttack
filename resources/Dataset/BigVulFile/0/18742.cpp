static void dccp_v4_ctl_send_reset(struct sock *sk, struct sk_buff *rxskb)
{
	int err;
	const struct iphdr *rxiph;
	struct sk_buff *skb;
	struct dst_entry *dst;
	struct net *net = dev_net(skb_dst(rxskb)->dev);
	struct sock *ctl_sk = net->dccp.v4_ctl_sk;

	/* Never send a reset in response to a reset. */
	if (dccp_hdr(rxskb)->dccph_type == DCCP_PKT_RESET)
		return;

	if (skb_rtable(rxskb)->rt_type != RTN_LOCAL)
		return;

	dst = dccp_v4_route_skb(net, ctl_sk, rxskb);
	if (dst == NULL)
		return;

	skb = dccp_ctl_make_reset(ctl_sk, rxskb);
	if (skb == NULL)
		goto out;

	rxiph = ip_hdr(rxskb);
	dccp_hdr(skb)->dccph_checksum = dccp_v4_csum_finish(skb, rxiph->saddr,
								 rxiph->daddr);
	skb_dst_set(skb, dst_clone(dst));

	bh_lock_sock(ctl_sk);
	err = ip_build_and_send_pkt(skb, ctl_sk,
				    rxiph->daddr, rxiph->saddr, NULL);
	bh_unlock_sock(ctl_sk);

	if (net_xmit_eval(err) == 0) {
		DCCP_INC_STATS_BH(DCCP_MIB_OUTSEGS);
		DCCP_INC_STATS_BH(DCCP_MIB_OUTRSTS);
	}
out:
	 dst_release(dst);
}
