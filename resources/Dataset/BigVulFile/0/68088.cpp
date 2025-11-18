static char *dex_class_super_name(RBinDexObj *bin, RBinDexClass *c) {
	int cid, tid;
	if (!bin || !c || !bin->types) {
		return NULL;
	}
	cid = c->super_class;
	if (cid < 0 || cid >= bin->header.types_size) {
		return NULL;
	}
	tid = bin->types[cid].descriptor_id;
	return getstr (bin, tid);
}
