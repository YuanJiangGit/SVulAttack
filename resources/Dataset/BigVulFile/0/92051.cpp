static void freed_request(struct request_list *rl, bool sync,
		req_flags_t rq_flags)
{
	struct request_queue *q = rl->q;

	q->nr_rqs[sync]--;
	rl->count[sync]--;
	if (rq_flags & RQF_ELVPRIV)
		q->nr_rqs_elvpriv--;

	__freed_request(rl, sync);

	if (unlikely(rl->starved[sync ^ 1]))
		__freed_request(rl, sync ^ 1);
}
