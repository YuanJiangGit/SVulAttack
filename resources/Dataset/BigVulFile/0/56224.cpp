static int callchain_trace(struct stackframe *frame, void *data)
{
	struct perf_callchain_entry *entry = data;
	perf_callchain_store(entry, frame->pc);
	return 0;
}
