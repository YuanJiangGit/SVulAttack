PHP_FUNCTION(getservbyname)
{
	char *name, *proto;
	int name_len, proto_len;
	struct servent *serv;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &name, &name_len, &proto, &proto_len) == FAILURE) {
		return;
	}


/* empty string behaves like NULL on windows implementation of 
   getservbyname. Let be portable instead. */
#ifdef PHP_WIN32
	if (proto_len == 0) {
		RETURN_FALSE;
	}
#endif

	serv = getservbyname(name, proto);

	if (serv == NULL) {
		RETURN_FALSE;
	}

	RETURN_LONG(ntohs(serv->s_port));
}
