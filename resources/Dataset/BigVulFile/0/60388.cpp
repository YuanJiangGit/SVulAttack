static struct bnep_session *__bnep_get_session(u8 *dst)
{
	struct bnep_session *s;

	BT_DBG("");

	list_for_each_entry(s, &bnep_session_list, list)
		if (ether_addr_equal(dst, s->eh.h_source))
			return s;

	return NULL;
}
