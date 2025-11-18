static const char *set_server_root(cmd_parms *cmd, void *dummy,
                                   const char *arg)
{
    const char *err = ap_check_cmd_context(cmd, GLOBAL_ONLY);

    if (err != NULL) {
        return err;
    }

    if ((apr_filepath_merge((char**)&ap_server_root, NULL, arg,
                            APR_FILEPATH_TRUENAME, cmd->pool) != APR_SUCCESS)
        || !ap_is_directory(cmd->temp_pool, ap_server_root)) {
        return "ServerRoot must be a valid directory";
    }

    return NULL;
}
