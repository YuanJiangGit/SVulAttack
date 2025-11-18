void nl80211_send_unprot_deauth(struct cfg80211_registered_device *rdev,
				struct net_device *netdev, const u8 *buf,
				size_t len, gfp_t gfp)
{
	nl80211_send_mlme_event(rdev, netdev, buf, len,
				NL80211_CMD_UNPROT_DEAUTHENTICATE, gfp);
}
