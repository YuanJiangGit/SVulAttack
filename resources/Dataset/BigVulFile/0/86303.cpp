static int rtnl_net_dumpid_one(int id, void *peer, void *data)
{
	struct rtnl_net_dump_cb *net_cb = (struct rtnl_net_dump_cb *)data;
	int ret;

	if (net_cb->idx < net_cb->s_idx)
		goto cont;

	ret = rtnl_net_fill(net_cb->skb, NETLINK_CB(net_cb->cb->skb).portid,
			    net_cb->cb->nlh->nlmsg_seq, NLM_F_MULTI,
			    RTM_NEWNSID, net_cb->net, id);
	if (ret < 0)
		return ret;

cont:
	net_cb->idx++;
	return 0;
}
