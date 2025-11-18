dissect_spoolss_relstrarray(tvbuff_t *tvb, int offset, packet_info *pinfo,
			    proto_tree *tree, dcerpc_info *di, guint8 *drep, int hf_index,
			    int struct_start, char **data)
{
	proto_item *item;
	proto_tree *subtree;
	guint32 relstr_offset, relstr_start/*, relstr_end, relstr_len*/;
	char *text;

	item = proto_tree_add_string(tree, hf_index, tvb, offset, 4, "");

	subtree = proto_item_add_subtree(item, ett_RELSTR_ARRAY);

	offset = dissect_ndr_uint32(
		tvb, offset, pinfo, subtree, di, drep, hf_offset, &relstr_offset);

	/* A relative offset of zero is a NULL string */

	relstr_start = relstr_offset + struct_start;

	if (relstr_offset)
		/*relstr_end = */dissect_spoolss_uint16uni(
			tvb, relstr_start, pinfo, subtree, drep, &text, hf_relative_string);
	else {
		text = g_strdup("NULL");
		/*relstr_end = offset;*/
	}

	/*relstr_len = relstr_end - relstr_start;*/

	proto_item_append_text(item, "%s", text);

	if (data)
		*data = text;
	else
		g_free(text);

	return offset;
}
