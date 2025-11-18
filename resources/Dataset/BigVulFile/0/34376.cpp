static struct extent_map *create_pinned_em(struct inode *inode, u64 start,
					   u64 len, u64 orig_start,
					   u64 block_start, u64 block_len,
					   u64 orig_block_len, int type)
{
	struct extent_map_tree *em_tree;
	struct extent_map *em;
	struct btrfs_root *root = BTRFS_I(inode)->root;
	int ret;

	em_tree = &BTRFS_I(inode)->extent_tree;
	em = alloc_extent_map();
	if (!em)
		return ERR_PTR(-ENOMEM);

	em->start = start;
	em->orig_start = orig_start;
	em->len = len;
	em->block_len = block_len;
	em->block_start = block_start;
	em->bdev = root->fs_info->fs_devices->latest_bdev;
	em->orig_block_len = orig_block_len;
	em->generation = -1;
	set_bit(EXTENT_FLAG_PINNED, &em->flags);
	if (type == BTRFS_ORDERED_PREALLOC)
		set_bit(EXTENT_FLAG_FILLING, &em->flags);

	do {
		btrfs_drop_extent_cache(inode, em->start,
				em->start + em->len - 1, 0);
		write_lock(&em_tree->lock);
		ret = add_extent_mapping(em_tree, em);
		if (!ret)
			list_move(&em->list,
				  &em_tree->modified_extents);
		write_unlock(&em_tree->lock);
	} while (ret == -EEXIST);

	if (ret) {
		free_extent_map(em);
		return ERR_PTR(ret);
	}

	return em;
}
