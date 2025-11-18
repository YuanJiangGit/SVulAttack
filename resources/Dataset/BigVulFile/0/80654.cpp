void urn_del(GF_Box *s)
{
	GF_DataEntryURNBox *ptr = (GF_DataEntryURNBox *)s;
	if (ptr == NULL) return;
	if (ptr->location) gf_free(ptr->location);
	if (ptr->nameURN) gf_free(ptr->nameURN);
	gf_free(ptr);
}
