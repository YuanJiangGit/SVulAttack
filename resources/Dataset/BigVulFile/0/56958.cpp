static int fuse_readpages(struct file *file, struct address_space *mapping,
			  struct list_head *pages, unsigned nr_pages)
{
	struct inode *inode = mapping->host;
	struct fuse_conn *fc = get_fuse_conn(inode);
	struct fuse_fill_data data;
	int err;
	int nr_alloc = min_t(unsigned, nr_pages, FUSE_MAX_PAGES_PER_REQ);

	err = -EIO;
	if (is_bad_inode(inode))
		goto out;

	data.file = file;
	data.inode = inode;
	if (fc->async_read)
		data.req = fuse_get_req_for_background(fc, nr_alloc);
	else
		data.req = fuse_get_req(fc, nr_alloc);
	data.nr_pages = nr_pages;
	err = PTR_ERR(data.req);
	if (IS_ERR(data.req))
		goto out;

	err = read_cache_pages(mapping, pages, fuse_readpages_fill, &data);
	if (!err) {
		if (data.req->num_pages)
			fuse_send_readpages(data.req, file);
		else
			fuse_put_request(fc, data.req);
	}
out:
	return err;
}
