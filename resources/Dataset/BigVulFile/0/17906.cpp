void vnc_jobs_clear(VncState *vs)
{
    VncJob *job, *tmp;

    vnc_lock_queue(queue);
    QTAILQ_FOREACH_SAFE(job, &queue->jobs, next, tmp) {
        if (job->vs == vs || !vs) {
            QTAILQ_REMOVE(&queue->jobs, job, next);
        }
    }
    vnc_unlock_queue(queue);
}
