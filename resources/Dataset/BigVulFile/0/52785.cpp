static void __exit ib_ucm_cleanup(void)
{
	ib_unregister_client(&ucm_client);
	class_remove_file(&cm_class, &class_attr_abi_version.attr);
	unregister_chrdev_region(IB_UCM_BASE_DEV, IB_UCM_MAX_DEVICES);
	if (overflow_maj)
		unregister_chrdev_region(overflow_maj, IB_UCM_MAX_DEVICES);
	idr_destroy(&ctx_id_table);
}
