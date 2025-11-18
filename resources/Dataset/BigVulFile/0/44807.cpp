local void log_add(char *fmt, ...)
{
    struct timeval now;
    struct log *me;
    va_list ap;
    char msg[MAXMSG];

    gettimeofday(&now, NULL);
    me = MALLOC(sizeof(struct log));
    if (me == NULL)
        bail("not enough memory", "");
    me->when = now;
    va_start(ap, fmt);
    vsnprintf(msg, MAXMSG, fmt, ap);
    va_end(ap);
    me->msg = MALLOC(strlen(msg) + 1);
    if (me->msg == NULL) {
        FREE(me);
        bail("not enough memory", "");
    }
    strcpy(me->msg, msg);
    me->next = NULL;
#ifndef NOTHREAD
    assert(log_lock != NULL);
    possess(log_lock);
#endif
    *log_tail = me;
    log_tail = &(me->next);
#ifndef NOTHREAD
    twist(log_lock, BY, +1);
#endif
}
