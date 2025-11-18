static int do_set_master(struct net_device *dev, int ifindex)
{
	struct net_device *upper_dev = netdev_master_upper_dev_get(dev);
	const struct net_device_ops *ops;
	int err;

	if (upper_dev) {
		if (upper_dev->ifindex == ifindex)
			return 0;
		ops = upper_dev->netdev_ops;
		if (ops->ndo_del_slave) {
			err = ops->ndo_del_slave(upper_dev, dev);
			if (err)
				return err;
		} else {
			return -EOPNOTSUPP;
		}
	}

	if (ifindex) {
		upper_dev = __dev_get_by_index(dev_net(dev), ifindex);
		if (!upper_dev)
			return -EINVAL;
		ops = upper_dev->netdev_ops;
		if (ops->ndo_add_slave) {
			err = ops->ndo_add_slave(upper_dev, dev);
			if (err)
				return err;
		} else {
			return -EOPNOTSUPP;
		}
	}
	return 0;
}
