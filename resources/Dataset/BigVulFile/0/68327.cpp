static bool is_kernel_event(struct perf_event *event)
{
	return READ_ONCE(event->owner) == TASK_TOMBSTONE;
}
