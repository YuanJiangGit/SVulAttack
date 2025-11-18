request_swap_ns(struct request *req, struct nameserver *ns) {
	if (ns && req->ns != ns) {
		EVUTIL_ASSERT(req->ns->requests_inflight > 0);
		req->ns->requests_inflight--;
		ns->requests_inflight++;

		req->ns = ns;
	}
}
