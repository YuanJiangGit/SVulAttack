static void process_level0_unix_area(LHAFileHeader *header,
                                     uint8_t *data, size_t data_len)
{


	if (data_len < LEVEL_0_UNIX_EXTENDED_LEN || data[1] != 0x00) {
		return;
	}



	header->os_type = data[0];
	header->timestamp = lha_decode_uint32(data + 2);

	header->unix_perms = lha_decode_uint16(data + data_len - 6);
	header->unix_uid = lha_decode_uint16(data + data_len - 4);
	header->unix_gid = lha_decode_uint16(data + data_len - 2);

	header->extra_flags |= LHA_FILE_UNIX_PERMS | LHA_FILE_UNIX_UID_GID;
}
