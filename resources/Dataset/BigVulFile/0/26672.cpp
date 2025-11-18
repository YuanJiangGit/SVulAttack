static int nl80211_del_key(struct sk_buff *skb, struct genl_info *info)
{
	struct cfg80211_registered_device *rdev = info->user_ptr[0];
	int err;
	struct net_device *dev = info->user_ptr[1];
	u8 *mac_addr = NULL;
	struct key_parse key;

	err = nl80211_parse_key(info, &key);
	if (err)
		return err;

	if (info->attrs[NL80211_ATTR_MAC])
		mac_addr = nla_data(info->attrs[NL80211_ATTR_MAC]);

	if (key.type == -1) {
		if (mac_addr)
			key.type = NL80211_KEYTYPE_PAIRWISE;
		else
			key.type = NL80211_KEYTYPE_GROUP;
	}

	/* for now */
	if (key.type != NL80211_KEYTYPE_PAIRWISE &&
	    key.type != NL80211_KEYTYPE_GROUP)
		return -EINVAL;

	if (!rdev->ops->del_key)
		return -EOPNOTSUPP;

	wdev_lock(dev->ieee80211_ptr);
	err = nl80211_key_allowed(dev->ieee80211_ptr);

	if (key.type == NL80211_KEYTYPE_PAIRWISE && mac_addr &&
	    !(rdev->wiphy.flags & WIPHY_FLAG_IBSS_RSN))
		err = -ENOENT;

	if (!err)
		err = rdev->ops->del_key(&rdev->wiphy, dev, key.idx,
					 key.type == NL80211_KEYTYPE_PAIRWISE,
					 mac_addr);

#ifdef CONFIG_CFG80211_WEXT
	if (!err) {
		if (key.idx == dev->ieee80211_ptr->wext.default_key)
			dev->ieee80211_ptr->wext.default_key = -1;
		else if (key.idx == dev->ieee80211_ptr->wext.default_mgmt_key)
			dev->ieee80211_ptr->wext.default_mgmt_key = -1;
	}
#endif
	wdev_unlock(dev->ieee80211_ptr);

	return err;
}
