R_API RFlag *r_flag_free(RFlag *f) {
	int i;
	for (i = 0; i < R_FLAG_SPACES_MAX; i++) {
		free (f->spaces[i]);
	}
	r_skiplist_free (f->by_off);
	ht_free (f->ht_name);

	r_list_free (f->flags);
	r_list_free (f->spacestack);
	r_num_free (f->num);
	free (f);
	return NULL;
}
