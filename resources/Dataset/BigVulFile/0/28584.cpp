static int qeth_init_qdio_out_buf(struct qeth_qdio_out_q *q, int bidx)
{
	int rc;
	struct qeth_qdio_out_buffer *newbuf;

	rc = 0;
	newbuf = kmem_cache_zalloc(qeth_qdio_outbuf_cache, GFP_ATOMIC);
	if (!newbuf) {
		rc = -ENOMEM;
		goto out;
	}
	newbuf->buffer = &q->qdio_bufs[bidx];
	skb_queue_head_init(&newbuf->skb_list);
	lockdep_set_class(&newbuf->skb_list.lock, &qdio_out_skb_queue_key);
	newbuf->q = q;
	newbuf->aob = NULL;
	newbuf->next_pending = q->bufs[bidx];
	atomic_set(&newbuf->state, QETH_QDIO_BUF_EMPTY);
	q->bufs[bidx] = newbuf;
	if (q->bufstates) {
		q->bufstates[bidx].user = newbuf;
		QETH_CARD_TEXT_(q->card, 2, "nbs%d", bidx);
		QETH_CARD_TEXT_(q->card, 2, "%lx", (long) newbuf);
		QETH_CARD_TEXT_(q->card, 2, "%lx",
				(long) newbuf->next_pending);
	}
out:
	return rc;
}
