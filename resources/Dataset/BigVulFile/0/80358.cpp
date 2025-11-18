GF_Box *rtp_hnti_New()
{
	ISOM_DECL_BOX_ALLOC(GF_RTPBox, GF_ISOM_BOX_TYPE_RTP);
	tmp->subType = GF_ISOM_BOX_TYPE_SDP;
	return (GF_Box *)tmp;
}
