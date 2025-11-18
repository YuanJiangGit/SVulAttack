void xen_pcibk_config_free_dev(struct pci_dev *dev)
{
	struct xen_pcibk_dev_data *dev_data = pci_get_drvdata(dev);
	struct config_field_entry *cfg_entry, *t;
	const struct config_field *field;

	dev_dbg(&dev->dev, "free-ing virtual configuration space fields\n");
	if (!dev_data)
		return;

	list_for_each_entry_safe(cfg_entry, t, &dev_data->config_fields, list) {
		list_del(&cfg_entry->list);

		field = cfg_entry->field;

		if (field->release)
			field->release(dev, OFFSET(cfg_entry), cfg_entry->data);

		kfree(cfg_entry);
	}
}
