static void srpt_rdma_write_done(struct ib_cq *cq, struct ib_wc *wc)
{
	struct srpt_send_ioctx *ioctx =
		container_of(wc->wr_cqe, struct srpt_send_ioctx, rdma_cqe);

	if (unlikely(wc->status != IB_WC_SUCCESS)) {
		pr_info("RDMA_WRITE for ioctx 0x%p failed with status %d\n",
			ioctx, wc->status);
		srpt_abort_cmd(ioctx);
	}
}
