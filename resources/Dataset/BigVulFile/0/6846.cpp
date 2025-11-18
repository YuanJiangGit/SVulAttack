http_reply_and_close(struct stream *s, short status, struct chunk *msg)
{
	s->txn->flags &= ~TX_WAIT_NEXT_RQ;
	FLT_STRM_CB(s, flt_http_reply(s, status, msg));
	stream_int_retnclose(&s->si[0], msg);
}
