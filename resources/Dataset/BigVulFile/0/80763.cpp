GF_Err iSFM_dump(GF_Box *a, FILE * trace)
{
	GF_ISMASampleFormatBox *p;
	const char *name = (a->type==GF_ISOM_BOX_TYPE_ISFM) ? "ISMASampleFormat" : "OMADRMAUFormatBox";
	p = (GF_ISMASampleFormatBox *)a;
	gf_isom_box_dump_start(a, name, trace);
	fprintf(trace, "selective_encryption=\"%d\" key_indicator_length=\"%d\" IV_length=\"%d\">\n", p->selective_encryption, p->key_indicator_length, p->IV_length);
	gf_isom_box_dump_done(name, a, trace);
	return GF_OK;
}
