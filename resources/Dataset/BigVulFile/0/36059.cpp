static int udf_read_extent_cache(struct inode *inode, loff_t bcount,
				 loff_t *lbcount, struct extent_position *pos)
{
	struct udf_inode_info *iinfo = UDF_I(inode);
	int ret = 0;

	spin_lock(&iinfo->i_extent_cache_lock);
	if ((iinfo->cached_extent.lstart <= bcount) &&
	    (iinfo->cached_extent.lstart != -1)) {
		/* Cache hit */
		*lbcount = iinfo->cached_extent.lstart;
		memcpy(pos, &iinfo->cached_extent.epos,
		       sizeof(struct extent_position));
		if (pos->bh)
			get_bh(pos->bh);
		ret = 1;
	}
	spin_unlock(&iinfo->i_extent_cache_lock);
	return ret;
}
