static void *m_start(struct seq_file *m, loff_t *pos)
{
	struct proc_mounts *p = proc_mounts(m);

	down_read(&namespace_sem);
	if (p->cached_event == p->ns->event) {
		void *v = p->cached_mount;
		if (*pos == p->cached_index)
			return v;
		if (*pos == p->cached_index + 1) {
			v = seq_list_next(v, &p->ns->list, &p->cached_index);
			return p->cached_mount = v;
		}
	}

	p->cached_event = p->ns->event;
	p->cached_mount = seq_list_start(&p->ns->list, *pos);
	p->cached_index = *pos;
	return p->cached_mount;
}
