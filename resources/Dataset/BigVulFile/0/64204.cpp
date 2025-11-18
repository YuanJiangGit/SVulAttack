AP_DECLARE(int) ap_is_recursion_limit_exceeded(const request_rec *r)
{
    core_server_config *conf =
        ap_get_core_module_config(r->server->module_config);
    const request_rec *top = r;
    int redirects = 0, subreqs = 0;
    int rlimit = conf->redirect_limit
                 ? conf->redirect_limit
                 : AP_DEFAULT_MAX_INTERNAL_REDIRECTS;
    int slimit = conf->subreq_limit
                 ? conf->subreq_limit
                 : AP_DEFAULT_MAX_SUBREQ_DEPTH;


    while (top->prev || top->main) {
        if (top->prev) {
            if (++redirects >= rlimit) {
                /* uuh, too much. */
                ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, APLOGNO(00124)
                              "Request exceeded the limit of %d internal "
                              "redirects due to probable configuration error. "
                              "Use 'LimitInternalRecursion' to increase the "
                              "limit if necessary. Use 'LogLevel debug' to get "
                              "a backtrace.", rlimit);

                /* post backtrace */
                log_backtrace(r);

                /* return failure */
                return 1;
            }

            top = top->prev;
        }

        if (!top->prev && top->main) {
            if (++subreqs >= slimit) {
                /* uuh, too much. */
                ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, APLOGNO(00125)
                              "Request exceeded the limit of %d subrequest "
                              "nesting levels due to probable configuration "
                              "error. Use 'LimitInternalRecursion' to increase "
                              "the limit if necessary. Use 'LogLevel debug' to "
                              "get a backtrace.", slimit);

                /* post backtrace */
                log_backtrace(r);

                /* return failure */
                return 1;
            }

            top = top->main;
        }
    }

    /* recursion state: ok */
    return 0;
}
