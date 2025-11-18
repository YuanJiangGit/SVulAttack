struct hid_device *hid_allocate_device(void)
{
	struct hid_device *hdev;
	int ret = -ENOMEM;

	hdev = kzalloc(sizeof(*hdev), GFP_KERNEL);
	if (hdev == NULL)
		return ERR_PTR(ret);

	device_initialize(&hdev->dev);
	hdev->dev.release = hid_device_release;
	hdev->dev.bus = &hid_bus_type;
	device_enable_async_suspend(&hdev->dev);

	hid_close_report(hdev);

	init_waitqueue_head(&hdev->debug_wait);
	INIT_LIST_HEAD(&hdev->debug_list);
	spin_lock_init(&hdev->debug_list_lock);
	sema_init(&hdev->driver_lock, 1);
	sema_init(&hdev->driver_input_lock, 1);

	return hdev;
}
