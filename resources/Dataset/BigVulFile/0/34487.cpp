static noinline int commit_fs_roots(struct btrfs_trans_handle *trans,
				    struct btrfs_root *root)
{
	struct btrfs_root *gang[8];
	struct btrfs_fs_info *fs_info = root->fs_info;
	int i;
	int ret;
	int err = 0;

	spin_lock(&fs_info->fs_roots_radix_lock);
	while (1) {
		ret = radix_tree_gang_lookup_tag(&fs_info->fs_roots_radix,
						 (void **)gang, 0,
						 ARRAY_SIZE(gang),
						 BTRFS_ROOT_TRANS_TAG);
		if (ret == 0)
			break;
		for (i = 0; i < ret; i++) {
			root = gang[i];
			radix_tree_tag_clear(&fs_info->fs_roots_radix,
					(unsigned long)root->root_key.objectid,
					BTRFS_ROOT_TRANS_TAG);
			spin_unlock(&fs_info->fs_roots_radix_lock);

			btrfs_free_log(trans, root);
			btrfs_update_reloc_root(trans, root);
			btrfs_orphan_commit_root(trans, root);

			btrfs_save_ino_cache(root, trans);

			/* see comments in should_cow_block() */
			root->force_cow = 0;
			smp_wmb();

			if (root->commit_root != root->node) {
				mutex_lock(&root->fs_commit_mutex);
				switch_commit_root(root);
				btrfs_unpin_free_ino(root);
				mutex_unlock(&root->fs_commit_mutex);

				btrfs_set_root_node(&root->root_item,
						    root->node);
			}

			err = btrfs_update_root(trans, fs_info->tree_root,
						&root->root_key,
						&root->root_item);
			spin_lock(&fs_info->fs_roots_radix_lock);
			if (err)
				break;
		}
	}
	spin_unlock(&fs_info->fs_roots_radix_lock);
	return err;
}
