evutil_issetugid(void)
{
#ifdef EVENT__HAVE_ISSETUGID
	return issetugid();
#else

#ifdef EVENT__HAVE_GETEUID
	if (getuid() != geteuid())
		return 1;
#endif
#ifdef EVENT__HAVE_GETEGID
	if (getgid() != getegid())
		return 1;
#endif
	return 0;
#endif
}
