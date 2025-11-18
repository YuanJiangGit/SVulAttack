issueLocalRequest(int request, void *data, int len, RIL_SOCKET_ID socket_id) {
 RequestInfo *pRI;
 int ret;
 /* Hook for current context */
 /* pendingRequestsMutextHook refer to &s_pendingRequestsMutex */
 pthread_mutex_t* pendingRequestsMutexHook = &s_pendingRequestsMutex;
 /* pendingRequestsHook refer to &s_pendingRequests */
 RequestInfo**    pendingRequestsHook = &s_pendingRequests;

#if (SIM_COUNT == 2)
 if (socket_id == RIL_SOCKET_2) {
        pendingRequestsMutexHook = &s_pendingRequestsMutex_socket2;
        pendingRequestsHook = &s_pendingRequests_socket2;
 }
#endif

    pRI = (RequestInfo *)calloc(1, sizeof(RequestInfo));
 if (pRI == NULL) {
        RLOGE("Memory allocation failed for request %s", requestToString(request));
 return;
 }

    pRI->local = 1;
    pRI->token = 0xffffffff; // token is not used in this context
    pRI->pCI = &(s_commands[request]);
    pRI->socket_id = socket_id;

    ret = pthread_mutex_lock(pendingRequestsMutexHook);
    assert (ret == 0);

    pRI->p_next = *pendingRequestsHook;
 *pendingRequestsHook = pRI;

    ret = pthread_mutex_unlock(pendingRequestsMutexHook);
    assert (ret == 0);

    RLOGD("C[locl]> %s", requestToString(request));

    CALL_ONREQUEST(request, data, len, pRI, pRI->socket_id);
}
