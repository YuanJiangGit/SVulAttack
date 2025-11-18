wb_dops(netdissect_options *ndo, const struct pkt_dop *dop,
        uint32_t ss, uint32_t es)
{
	const struct dophdr *dh = (const struct dophdr *)((const u_char *)dop + sizeof(*dop));

	ND_PRINT((ndo, " <"));
	for ( ; ss <= es; ++ss) {
		int t;

		if (!ND_TTEST(*dh)) {
			ND_PRINT((ndo, "%s", tstr));
			break;
		}
		t = dh->dh_type;

		if (t > DT_MAXTYPE)
			ND_PRINT((ndo, " dop-%d!", t));
		else {
			ND_PRINT((ndo, " %s", dopstr[t]));
			if (t == DT_SKIP || t == DT_HOLE) {
				uint32_t ts = EXTRACT_32BITS(&dh->dh_ts);
				ND_PRINT((ndo, "%d", ts - ss + 1));
				if (ss > ts || ts > es) {
					ND_PRINT((ndo, "[|]"));
					if (ts < ss)
						return (0);
				}
				ss = ts;
			}
		}
		dh = DOP_NEXT(dh);
	}
	ND_PRINT((ndo, " >"));
	return (0);
}
