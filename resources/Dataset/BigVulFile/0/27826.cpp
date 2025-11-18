static void br_multicast_port_group_query_expired(unsigned long data)
{
	struct net_bridge_port_group *pg = (void *)data;
	struct net_bridge_port *port = pg->port;
	struct net_bridge *br = port->br;

	spin_lock(&br->multicast_lock);
	if (!netif_running(br->dev) || hlist_unhashed(&pg->mglist) ||
	    pg->queries_sent >= br->multicast_last_member_count)
		goto out;

	br_multicast_send_port_group_query(pg);

out:
	spin_unlock(&br->multicast_lock);
}
