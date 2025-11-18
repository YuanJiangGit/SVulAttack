void hns_ppe_set_rss_key(struct hns_ppe_cb *ppe_cb,
			 const u32 rss_key[HNS_PPEV2_RSS_KEY_NUM])
{
	u32 key_item;

	for (key_item = 0; key_item < HNS_PPEV2_RSS_KEY_NUM; key_item++)
		dsaf_write_dev(ppe_cb, PPEV2_RSS_KEY_REG + key_item * 0x4,
			       rss_key[key_item]);
}
