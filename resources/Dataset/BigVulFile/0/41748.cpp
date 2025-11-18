int addrconf_add_ifaddr(struct net *net, void __user *arg)
{
	struct in6_ifreq ireq;
	int err;

	if (!ns_capable(net->user_ns, CAP_NET_ADMIN))
		return -EPERM;

	if (copy_from_user(&ireq, arg, sizeof(struct in6_ifreq)))
		return -EFAULT;

	rtnl_lock();
	err = inet6_addr_add(net, ireq.ifr6_ifindex, &ireq.ifr6_addr, NULL,
			     ireq.ifr6_prefixlen, IFA_F_PERMANENT,
			     INFINITY_LIFE_TIME, INFINITY_LIFE_TIME);
	rtnl_unlock();
	return err;
}
