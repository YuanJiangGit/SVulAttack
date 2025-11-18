RIL_startEventLoop(void) {
 /* spin up eventLoop thread and wait for it to get started */
    s_started = 0;
    pthread_mutex_lock(&s_startupMutex);

 pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

 int result = pthread_create(&s_tid_dispatch, &attr, eventLoop, NULL);
 if (result != 0) {
        RLOGE("Failed to create dispatch thread: %s", strerror(result));
 goto done;
 }

 while (s_started == 0) {
        pthread_cond_wait(&s_startupCond, &s_startupMutex);
 }

done:
    pthread_mutex_unlock(&s_startupMutex);
}
