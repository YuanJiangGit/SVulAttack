static int iter_to_pipe(struct iov_iter *from,
			struct pipe_inode_info *pipe,
			unsigned flags)
{
	struct pipe_buffer buf = {
		.ops = &user_page_pipe_buf_ops,
		.flags = flags
	};
	size_t total = 0;
	int ret = 0;
	bool failed = false;

	while (iov_iter_count(from) && !failed) {
		struct page *pages[16];
		ssize_t copied;
		size_t start;
		int n;

		copied = iov_iter_get_pages(from, pages, ~0UL, 16, &start);
		if (copied <= 0) {
			ret = copied;
			break;
		}

		for (n = 0; copied; n++, start = 0) {
			int size = min_t(int, copied, PAGE_SIZE - start);
			if (!failed) {
				buf.page = pages[n];
				buf.offset = start;
				buf.len = size;
				ret = add_to_pipe(pipe, &buf);
				if (unlikely(ret < 0)) {
					failed = true;
				} else {
					iov_iter_advance(from, ret);
					total += ret;
				}
			} else {
				put_page(pages[n]);
			}
			copied -= size;
		}
	}
	return total ? total : ret;
}
