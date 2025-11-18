free_netinfo_config(
	struct netinfo_config_state *config
	)
{
	ni_free(config->domain);
	free(config);
}
