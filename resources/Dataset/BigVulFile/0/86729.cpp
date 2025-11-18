static void blk_mq_queue_reinit(struct request_queue *q)
{
	WARN_ON_ONCE(!atomic_read(&q->mq_freeze_depth));

	blk_mq_sysfs_unregister(q);

	blk_mq_update_queue_map(q->mq_map, q->nr_hw_queues);

	/*
	 * redo blk_mq_init_cpu_queues and blk_mq_init_hw_queues. FIXME: maybe
	 * we should change hctx numa_node according to new topology (this
	 * involves free and re-allocate memory, worthy doing?)
	 */

	blk_mq_map_swqueue(q);

	blk_mq_sysfs_register(q);
}
