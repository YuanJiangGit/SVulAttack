addrinfo_from_hostent(const struct hostent *ent,
    int port, const struct evutil_addrinfo *hints)
{
	int i;
	struct sockaddr_in sin;
	struct sockaddr_in6 sin6;
	struct sockaddr *sa;
	int socklen;
	struct evutil_addrinfo *res=NULL, *ai;
	void *addrp;

	if (ent->h_addrtype == PF_INET) {
		memset(&sin, 0, sizeof(sin));
		sin.sin_family = AF_INET;
		sin.sin_port = htons(port);
		sa = (struct sockaddr *)&sin;
		socklen = sizeof(struct sockaddr_in);
		addrp = &sin.sin_addr;
		if (ent->h_length != sizeof(sin.sin_addr)) {
			event_warnx("Weird h_length from gethostbyname");
			return NULL;
		}
	} else if (ent->h_addrtype == PF_INET6) {
		memset(&sin6, 0, sizeof(sin6));
		sin6.sin6_family = AF_INET6;
		sin6.sin6_port = htons(port);
		sa = (struct sockaddr *)&sin6;
		socklen = sizeof(struct sockaddr_in6);
		addrp = &sin6.sin6_addr;
		if (ent->h_length != sizeof(sin6.sin6_addr)) {
			event_warnx("Weird h_length from gethostbyname");
			return NULL;
		}
	} else
		return NULL;

	for (i = 0; ent->h_addr_list[i]; ++i) {
		memcpy(addrp, ent->h_addr_list[i], ent->h_length);
		ai = evutil_new_addrinfo_(sa, socklen, hints);
		if (!ai) {
			evutil_freeaddrinfo(res);
			return NULL;
		}
		res = evutil_addrinfo_append_(res, ai);
	}

	if (res && ((hints->ai_flags & EVUTIL_AI_CANONNAME) && ent->h_name)) {
		res->ai_canonname = mm_strdup(ent->h_name);
		if (res->ai_canonname == NULL) {
			evutil_freeaddrinfo(res);
			return NULL;
		}
	}

	return res;
}
