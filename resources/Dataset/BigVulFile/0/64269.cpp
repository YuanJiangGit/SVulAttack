static const char *set_async_filter(cmd_parms *cmd, void *dummy,
                                             const char *arg)
{
    core_server_config *conf =
    ap_get_core_module_config(cmd->server->module_config);
    const char *err = ap_check_cmd_context(cmd, NOT_IN_DIR_LOC_FILE);

    if (err) {
        return err;
    }

    if (ap_cstr_casecmp(arg, "network") == 0) {
        conf->async_filter = AP_FTYPE_NETWORK;
    }
    else if (ap_cstr_casecmp(arg, "connection") == 0) {
        conf->async_filter = AP_FTYPE_CONNECTION;
    }
    else if (ap_cstr_casecmp(arg, "request") == 0) {
        conf->async_filter = 0;
    }
    else {
        return "AsyncFilter must be 'network', 'connection' or 'request'";
    }
    conf->async_filter_set = 1;

    return NULL;
}
