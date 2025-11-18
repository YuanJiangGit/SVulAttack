perf_event_output_forward(struct perf_event *event,
			 struct perf_sample_data *data,
			 struct pt_regs *regs)
{
	__perf_event_output(event, data, regs, perf_output_begin_forward);
}
