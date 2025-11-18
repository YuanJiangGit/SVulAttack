static int jas_iccprof_puttagtab(jas_stream_t *out, jas_icctagtab_t *tagtab)
{
	int i;
	jas_icctagtabent_t *tagtabent;
	if (jas_iccputuint32(out, tagtab->numents))
		goto error;
	for (i = 0; i < JAS_CAST(int, tagtab->numents); ++i) {
		tagtabent = &tagtab->ents[i];
		if (jas_iccputuint32(out, tagtabent->tag) ||
		  jas_iccputuint32(out, tagtabent->off) ||
		  jas_iccputuint32(out, tagtabent->len))
			goto error;
	}
	return 0;
error:
	return -1;
}
