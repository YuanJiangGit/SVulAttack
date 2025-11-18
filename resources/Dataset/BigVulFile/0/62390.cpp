safeputs(netdissect_options *ndo,
         const u_char *s, const u_int maxlen)
{
	u_int idx = 0;

	while (idx < maxlen && *s) {
		safeputchar(ndo, *s);
		idx++;
		s++;
	}
}
