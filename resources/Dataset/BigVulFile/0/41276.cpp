static void comedi_device_init(struct comedi_device *dev)
{
	memset(dev, 0, sizeof(struct comedi_device));
	spin_lock_init(&dev->spinlock);
	mutex_init(&dev->mutex);
	dev->minor = -1;
}
