int ext4_register_li_request(struct super_block *sb,
			     ext4_group_t first_not_zeroed)
{
	struct ext4_sb_info *sbi = EXT4_SB(sb);
	struct ext4_li_request *elr = NULL;
	ext4_group_t ngroups = EXT4_SB(sb)->s_groups_count;
	int ret = 0;

	mutex_lock(&ext4_li_mtx);
	if (sbi->s_li_request != NULL) {
		/*
		 * Reset timeout so it can be computed again, because
		 * s_li_wait_mult might have changed.
		 */
		sbi->s_li_request->lr_timeout = 0;
		goto out;
	}

	if (first_not_zeroed == ngroups ||
	    (sb->s_flags & MS_RDONLY) ||
	    !test_opt(sb, INIT_INODE_TABLE))
		goto out;

	elr = ext4_li_request_new(sb, first_not_zeroed);
	if (!elr) {
		ret = -ENOMEM;
		goto out;
	}

	if (NULL == ext4_li_info) {
		ret = ext4_li_info_new();
		if (ret)
			goto out;
	}

	mutex_lock(&ext4_li_info->li_list_mtx);
	list_add(&elr->lr_request, &ext4_li_info->li_request_list);
	mutex_unlock(&ext4_li_info->li_list_mtx);

	sbi->s_li_request = elr;
	/*
	 * set elr to NULL here since it has been inserted to
	 * the request_list and the removal and free of it is
	 * handled by ext4_clear_request_list from now on.
	 */
	elr = NULL;

	if (!(ext4_li_info->li_state & EXT4_LAZYINIT_RUNNING)) {
		ret = ext4_run_lazyinit_thread();
		if (ret)
			goto out;
	}
out:
	mutex_unlock(&ext4_li_mtx);
	if (ret)
		kfree(elr);
	return ret;
}
