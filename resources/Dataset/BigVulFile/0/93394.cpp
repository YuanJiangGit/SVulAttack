void napi_busy_loop(unsigned int napi_id,
		    bool (*loop_end)(void *, unsigned long),
		    void *loop_end_arg)
{
	unsigned long start_time = loop_end ? busy_loop_current_time() : 0;
	int (*napi_poll)(struct napi_struct *napi, int budget);
	void *have_poll_lock = NULL;
	struct napi_struct *napi;

restart:
	napi_poll = NULL;

	rcu_read_lock();

	napi = napi_by_id(napi_id);
	if (!napi)
		goto out;

	preempt_disable();
	for (;;) {
		int work = 0;

		local_bh_disable();
		if (!napi_poll) {
			unsigned long val = READ_ONCE(napi->state);

			/* If multiple threads are competing for this napi,
			 * we avoid dirtying napi->state as much as we can.
			 */
			if (val & (NAPIF_STATE_DISABLE | NAPIF_STATE_SCHED |
				   NAPIF_STATE_IN_BUSY_POLL))
				goto count;
			if (cmpxchg(&napi->state, val,
				    val | NAPIF_STATE_IN_BUSY_POLL |
					  NAPIF_STATE_SCHED) != val)
				goto count;
			have_poll_lock = netpoll_poll_lock(napi);
			napi_poll = napi->poll;
		}
		work = napi_poll(napi, BUSY_POLL_BUDGET);
		trace_napi_poll(napi, work, BUSY_POLL_BUDGET);
count:
		if (work > 0)
			__NET_ADD_STATS(dev_net(napi->dev),
					LINUX_MIB_BUSYPOLLRXPACKETS, work);
		local_bh_enable();

		if (!loop_end || loop_end(loop_end_arg, start_time))
			break;

		if (unlikely(need_resched())) {
			if (napi_poll)
				busy_poll_stop(napi, have_poll_lock);
			preempt_enable();
			rcu_read_unlock();
			cond_resched();
			if (loop_end(loop_end_arg, start_time))
				return;
			goto restart;
		}
		cpu_relax();
	}
	if (napi_poll)
		busy_poll_stop(napi, have_poll_lock);
	preempt_enable();
out:
	rcu_read_unlock();
}
