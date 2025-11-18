static void blk_mq_rq_timer(unsigned long priv)
{
	struct request_queue *q = (struct request_queue *)priv;
	struct blk_mq_timeout_data data = {
		.next		= 0,
		.next_set	= 0,
	};
	struct blk_mq_hw_ctx *hctx;
	int i;

	queue_for_each_hw_ctx(q, hctx, i) {
		/*
		 * If not software queues are currently mapped to this
		 * hardware queue, there's nothing to check
		 */
		if (!blk_mq_hw_queue_mapped(hctx))
			continue;

		blk_mq_tag_busy_iter(hctx, blk_mq_check_expired, &data);
	}

	if (data.next_set) {
		data.next = blk_rq_timeout(round_jiffies_up(data.next));
		mod_timer(&q->timeout, data.next);
	} else {
		queue_for_each_hw_ctx(q, hctx, i) {
			/* the hctx may be unmapped, so check it here */
			if (blk_mq_hw_queue_mapped(hctx))
				blk_mq_tag_idle(hctx);
		}
	}
}
