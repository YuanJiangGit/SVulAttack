static void __freed_request(struct request_list *rl, int sync)
{
	struct request_queue *q = rl->q;

	if (rl->count[sync] < queue_congestion_off_threshold(q))
		blk_clear_congested(rl, sync);

	if (rl->count[sync] + 1 <= q->nr_requests) {
		if (waitqueue_active(&rl->wait[sync]))
			wake_up(&rl->wait[sync]);

		blk_clear_rl_full(rl, sync);
	}
}
