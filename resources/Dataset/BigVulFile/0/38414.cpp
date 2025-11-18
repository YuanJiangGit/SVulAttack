static void cm_release_port_obj(struct kobject *obj)
{
	struct cm_port *cm_port;

	cm_port = container_of(obj, struct cm_port, port_obj);
	kfree(cm_port);
}
