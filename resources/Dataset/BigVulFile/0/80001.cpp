GF_Err ccst_Read(GF_Box *s, GF_BitStream *bs)
{
	GF_CodingConstraintsBox *ptr = (GF_CodingConstraintsBox *)s;

	ISOM_DECREASE_SIZE(ptr, 4);
	ptr->all_ref_pics_intra = gf_bs_read_int(bs, 1);
	ptr->intra_pred_used = gf_bs_read_int(bs, 1);
	ptr->max_ref_per_pic = gf_bs_read_int(bs, 4);
	ptr->reserved = gf_bs_read_int(bs, 26);
	return GF_OK;
}
