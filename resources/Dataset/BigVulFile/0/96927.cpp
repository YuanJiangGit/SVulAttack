tracing_snapshot_write(struct file *filp, const char __user *ubuf, size_t cnt,
		       loff_t *ppos)
{
	struct seq_file *m = filp->private_data;
	struct trace_iterator *iter = m->private;
	struct trace_array *tr = iter->tr;
	unsigned long val;
	int ret;

	ret = tracing_update_buffers();
	if (ret < 0)
		return ret;

	ret = kstrtoul_from_user(ubuf, cnt, 10, &val);
	if (ret)
		return ret;

	mutex_lock(&trace_types_lock);

	if (tr->current_trace->use_max_tr) {
		ret = -EBUSY;
		goto out;
	}

	arch_spin_lock(&tr->max_lock);
	if (tr->cond_snapshot)
		ret = -EBUSY;
	arch_spin_unlock(&tr->max_lock);
	if (ret)
		goto out;

	switch (val) {
	case 0:
		if (iter->cpu_file != RING_BUFFER_ALL_CPUS) {
			ret = -EINVAL;
			break;
		}
		if (tr->allocated_snapshot)
			free_snapshot(tr);
		break;
	case 1:
/* Only allow per-cpu swap if the ring buffer supports it */
#ifndef CONFIG_RING_BUFFER_ALLOW_SWAP
		if (iter->cpu_file != RING_BUFFER_ALL_CPUS) {
			ret = -EINVAL;
			break;
		}
#endif
		if (!tr->allocated_snapshot) {
			ret = tracing_alloc_snapshot_instance(tr);
			if (ret < 0)
				break;
		}
		local_irq_disable();
		/* Now, we're going to swap */
		if (iter->cpu_file == RING_BUFFER_ALL_CPUS)
			update_max_tr(tr, current, smp_processor_id(), NULL);
		else
			update_max_tr_single(tr, current, iter->cpu_file);
		local_irq_enable();
		break;
	default:
		if (tr->allocated_snapshot) {
			if (iter->cpu_file == RING_BUFFER_ALL_CPUS)
				tracing_reset_online_cpus(&tr->max_buffer);
			else
				tracing_reset(&tr->max_buffer, iter->cpu_file);
		}
		break;
	}

	if (ret >= 0) {
		*ppos += cnt;
		ret = cnt;
	}
out:
	mutex_unlock(&trace_types_lock);
	return ret;
}
