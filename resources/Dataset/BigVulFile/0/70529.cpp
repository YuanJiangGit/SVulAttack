static int ext4_feature_set_ok(struct super_block *sb, int readonly)
{
	if (ext4_has_unknown_ext4_incompat_features(sb)) {
		ext4_msg(sb, KERN_ERR,
			"Couldn't mount because of "
			"unsupported optional features (%x)",
			(le32_to_cpu(EXT4_SB(sb)->s_es->s_feature_incompat) &
			~EXT4_FEATURE_INCOMPAT_SUPP));
		return 0;
	}

	if (readonly)
		return 1;

	if (ext4_has_feature_readonly(sb)) {
		ext4_msg(sb, KERN_INFO, "filesystem is read-only");
		sb->s_flags |= MS_RDONLY;
		return 1;
	}

	/* Check that feature set is OK for a read-write mount */
	if (ext4_has_unknown_ext4_ro_compat_features(sb)) {
		ext4_msg(sb, KERN_ERR, "couldn't mount RDWR because of "
			 "unsupported optional features (%x)",
			 (le32_to_cpu(EXT4_SB(sb)->s_es->s_feature_ro_compat) &
				~EXT4_FEATURE_RO_COMPAT_SUPP));
		return 0;
	}
	/*
	 * Large file size enabled file system can only be mounted
	 * read-write on 32-bit systems if kernel is built with CONFIG_LBDAF
	 */
	if (ext4_has_feature_huge_file(sb)) {
		if (sizeof(blkcnt_t) < sizeof(u64)) {
			ext4_msg(sb, KERN_ERR, "Filesystem with huge files "
				 "cannot be mounted RDWR without "
				 "CONFIG_LBDAF");
			return 0;
		}
	}
	if (ext4_has_feature_bigalloc(sb) && !ext4_has_feature_extents(sb)) {
		ext4_msg(sb, KERN_ERR,
			 "Can't support bigalloc feature without "
			 "extents feature\n");
		return 0;
	}

#ifndef CONFIG_QUOTA
	if (ext4_has_feature_quota(sb) && !readonly) {
		ext4_msg(sb, KERN_ERR,
			 "Filesystem with quota feature cannot be mounted RDWR "
			 "without CONFIG_QUOTA");
		return 0;
	}
	if (ext4_has_feature_project(sb) && !readonly) {
		ext4_msg(sb, KERN_ERR,
			 "Filesystem with project quota feature cannot be mounted RDWR "
			 "without CONFIG_QUOTA");
		return 0;
	}
#endif  /* CONFIG_QUOTA */
	return 1;
}
