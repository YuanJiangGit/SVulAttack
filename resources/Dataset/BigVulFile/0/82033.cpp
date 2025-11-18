 R_API RList *r_anal_bb_list_new() {
	RList *list = r_list_newf ((RListFree)r_anal_bb_free);
	if (!list) {
		return NULL;
	}
	return list;
}
