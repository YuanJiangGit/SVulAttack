static String* Pe_r_bin_pe_parse_string(struct PE_(r_bin_pe_obj_t)* bin, PE_DWord* curAddr) {
	String* string = calloc (1, sizeof(*string));
	PE_DWord begAddr = *curAddr;
	int len_value = 0;
	int i = 0;
	if (!string) {
		bprintf ("Warning: calloc (String)\n");
		return NULL;
	}
	if (begAddr > bin->size || begAddr + sizeof(string->wLength) > bin->size) {
		free_String (string);
		return NULL;
	}
	if (r_buf_read_at (bin->b, *curAddr, (ut8*) &string->wLength, sizeof(string->wLength)) != sizeof(string->wLength)) {
		bprintf ("Warning: read (String wLength)\n");
		goto out_error;
	}
	*curAddr += sizeof(string->wLength);
	if (*curAddr > bin->size || *curAddr + sizeof(string->wValueLength) > bin->size) {
		goto out_error;
	}
	if (r_buf_read_at (bin->b, *curAddr, (ut8*) &string->wValueLength, sizeof(string->wValueLength)) != sizeof(string->wValueLength)) {
		bprintf ("Warning: read (String wValueLength)\n");
		goto out_error;
	}
	*curAddr += sizeof(string->wValueLength);

	if (*curAddr > bin->size || *curAddr + sizeof(string->wType) > bin->size) {
		goto out_error;
	}
	if (r_buf_read_at (bin->b, *curAddr, (ut8*) &string->wType, sizeof(string->wType)) != sizeof(string->wType)) {
		bprintf ("Warning: read (String wType)\n");
		goto out_error;
	}
	*curAddr += sizeof(string->wType);
	if (string->wType != 0 && string->wType != 1) {
		bprintf ("Warning: check (String wType)\n");
		goto out_error;
	}

	for (i = 0; *curAddr < begAddr + string->wLength; ++i, *curAddr += sizeof (ut16)) {
		ut16 utf16_char;
		if (*curAddr > bin->size || *curAddr + sizeof (ut16) > bin->size) {
			goto out_error;
		}
		if (r_buf_read_at (bin->b, *curAddr, (ut8*) &utf16_char, sizeof (ut16)) != sizeof (ut16)) {
			bprintf ("Warning: check (String szKey)\n");
			goto out_error;
		}
		string->szKey = (ut16*) realloc (string->szKey, (i + 1) * sizeof (ut16));
		string->szKey[i] = utf16_char;
		string->wKeyLen += sizeof (ut16);
		if (!utf16_char) {
			*curAddr += sizeof (ut16);
			break;
		}
	}
	align32 (*curAddr);
	len_value = R_MIN (string->wValueLength * 2, string->wLength - (*curAddr - begAddr));
	string->wValueLength = len_value;
	if (len_value < 0) {
		len_value = 0;
	}
	string->Value = (ut16*) calloc (len_value + 1, 1);
	if (!string->Value) {
		bprintf ("Warning: malloc (String Value)\n");
		goto out_error;
	}
	if (*curAddr > bin->size || *curAddr + len_value > bin->size) {
		goto out_error;
	}
	if (r_buf_read_at (bin->b, *curAddr, (ut8*) string->Value, len_value) != len_value) {
		bprintf ("Warning: read (String Value)\n");
		goto out_error;
	}
	*curAddr += len_value;
	return string;
out_error:
	free_String (string);
	return NULL;
}
