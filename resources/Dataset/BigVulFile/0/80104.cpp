GF_Err fdpa_Read(GF_Box *s, GF_BitStream *bs)
{
	u32 i;
	GF_FDpacketBox *ptr = (GF_FDpacketBox *)s;

	ISOM_DECREASE_SIZE(ptr, 3);
	ptr->info.sender_current_time_present = gf_bs_read_int(bs, 1);
	ptr->info.expected_residual_time_present = gf_bs_read_int(bs, 1);
	ptr->info.session_close_bit = gf_bs_read_int(bs, 1);
	ptr->info.object_close_bit = gf_bs_read_int(bs, 1);
	gf_bs_read_int(bs, 4);
	ptr->info.transport_object_identifier = gf_bs_read_u16(bs);
	ISOM_DECREASE_SIZE(ptr, 2);
	ptr->header_ext_count = gf_bs_read_u16(bs);
	if (ptr->header_ext_count*2>ptr->size) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Invalid number of entries %d in fdpa\n", ptr->header_ext_count));
		return GF_ISOM_INVALID_FILE;
	}

	GF_SAFE_ALLOC_N(ptr->headers, ptr->header_ext_count, GF_LCTheaderExtension);
	for (i=0; i<ptr->header_ext_count; i++) {
		ptr->headers[i].header_extension_type = gf_bs_read_u8(bs);
		ISOM_DECREASE_SIZE(ptr, 1);

		if (ptr->headers[i].header_extension_type > 127) {
			gf_bs_read_data(bs, (char *) ptr->headers[i].content, 3);
		} else {
			ISOM_DECREASE_SIZE(ptr, 1);
			ptr->headers[i].data_length = gf_bs_read_u8(bs);
			if (ptr->headers[i].data_length) {
				ptr->headers[i].data_length = 4*ptr->headers[i].data_length - 2;
				ptr->headers[i].data = gf_malloc(sizeof(char) * ptr->headers[i].data_length);
				gf_bs_read_data(bs, ptr->headers[i].data, ptr->headers[i].data_length);
			}
		}
	}
	return GF_OK;
}
