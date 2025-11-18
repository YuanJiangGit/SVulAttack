GF_Err kind_dump(GF_Box *a, FILE * trace)
{
	GF_KindBox *p;

	p = (GF_KindBox *)a;
	gf_isom_box_dump_start(a, "KindBox", trace);
	fprintf(trace, "schemeURI=\"%s\" value=\"%s\">\n", p->schemeURI, (p->value ? p->value : ""));
	gf_isom_box_dump_done("KindBox", a, trace);
	return GF_OK;
}
