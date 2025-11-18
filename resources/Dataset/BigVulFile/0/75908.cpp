start_keepalived(void)
{
	bool have_child = false;

#ifdef _WITH_BFD_
	/* must be opened before vrrp and bfd start */
	open_bfd_pipes();
#endif

#ifdef _WITH_LVS_
	/* start healthchecker child */
	if (running_checker()) {
		start_check_child();
		have_child = true;
	}
#endif
#ifdef _WITH_VRRP_
	/* start vrrp child */
	if (running_vrrp()) {
		start_vrrp_child();
		have_child = true;
	}
#endif
#ifdef _WITH_BFD_
	/* start bfd child */
	if (running_bfd()) {
		start_bfd_child();
		have_child = true;
	}
#endif

	return have_child;
}
