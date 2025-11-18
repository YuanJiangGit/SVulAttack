static void h2_session_ev_stream_done(h2_session *session, int arg, const char *msg)
{
    --session->open_streams;
    switch (session->state) {
        case H2_SESSION_ST_IDLE:
            if (session->open_streams == 0) {
                /* enter keepalive timeout, since we no longer have streams */
                session->idle_until = (session->s->keep_alive_timeout
                                       + apr_time_now());
            }
            break;
        default:
            break;
    }
}
