static struct se_node_acl *tcm_loop_tpg_alloc_fabric_acl(
	struct se_portal_group *se_tpg)
{
	struct tcm_loop_nacl *tl_nacl;

	tl_nacl = kzalloc(sizeof(struct tcm_loop_nacl), GFP_KERNEL);
	if (!tl_nacl) {
		printk(KERN_ERR "Unable to allocate struct tcm_loop_nacl\n");
		return NULL;
	}

	return &tl_nacl->se_node_acl;
}
