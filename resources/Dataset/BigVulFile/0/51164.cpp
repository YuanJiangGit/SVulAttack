static int audit_match_filetype(struct audit_context *ctx, int val)
{
	struct audit_names *n;
	umode_t mode = (umode_t)val;

	if (unlikely(!ctx))
		return 0;

	list_for_each_entry(n, &ctx->names_list, list) {
		if ((n->ino != AUDIT_INO_UNSET) &&
		    ((n->mode & S_IFMT) == mode))
			return 1;
	}

	return 0;
}
