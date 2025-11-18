static int nf_tables_fill_setelem(struct sk_buff *skb,
				  const struct nft_set *set,
				  const struct nft_set_elem *elem)
{
	unsigned char *b = skb_tail_pointer(skb);
	struct nlattr *nest;

	nest = nla_nest_start(skb, NFTA_LIST_ELEM);
	if (nest == NULL)
		goto nla_put_failure;

	if (nft_data_dump(skb, NFTA_SET_ELEM_KEY, &elem->key, NFT_DATA_VALUE,
			  set->klen) < 0)
		goto nla_put_failure;

	if (set->flags & NFT_SET_MAP &&
	    !(elem->flags & NFT_SET_ELEM_INTERVAL_END) &&
	    nft_data_dump(skb, NFTA_SET_ELEM_DATA, &elem->data,
			  set->dtype == NFT_DATA_VERDICT ? NFT_DATA_VERDICT : NFT_DATA_VALUE,
			  set->dlen) < 0)
		goto nla_put_failure;

	if (elem->flags != 0)
		if (nla_put_be32(skb, NFTA_SET_ELEM_FLAGS, htonl(elem->flags)))
			goto nla_put_failure;

	nla_nest_end(skb, nest);
	return 0;

nla_put_failure:
	nlmsg_trim(skb, b);
	return -EMSGSIZE;
}
