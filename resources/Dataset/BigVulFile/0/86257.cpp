tcpmss_tg4(struct sk_buff *skb, const struct xt_action_param *par)
{
	struct iphdr *iph = ip_hdr(skb);
	__be16 newlen;
	int ret;

	ret = tcpmss_mangle_packet(skb, par,
				   PF_INET,
				   iph->ihl * 4,
				   sizeof(*iph) + sizeof(struct tcphdr));
	if (ret < 0)
		return NF_DROP;
	if (ret > 0) {
		iph = ip_hdr(skb);
		newlen = htons(ntohs(iph->tot_len) + ret);
		csum_replace2(&iph->check, iph->tot_len, newlen);
		iph->tot_len = newlen;
	}
	return XT_CONTINUE;
}
