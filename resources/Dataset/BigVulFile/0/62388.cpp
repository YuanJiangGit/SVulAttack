print_unknown_data(netdissect_options *ndo, const u_char *cp,const char *ident,int len)
{
	if (len < 0) {
          ND_PRINT((ndo,"%sDissector error: print_unknown_data called with negative length",
		    ident));
		return(0);
	}
	if (ndo->ndo_snapend - cp < len)
		len = ndo->ndo_snapend - cp;
	if (len < 0) {
          ND_PRINT((ndo,"%sDissector error: print_unknown_data called with pointer past end of packet",
		    ident));
		return(0);
	}
        hex_print(ndo, ident,cp,len);
	return(1); /* everything is ok */
}
