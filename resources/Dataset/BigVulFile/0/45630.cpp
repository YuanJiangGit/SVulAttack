static int cryptd_aead_enqueue(struct aead_request *req,
				    crypto_completion_t compl)
{
	struct cryptd_aead_request_ctx *rctx = aead_request_ctx(req);
	struct crypto_aead *tfm = crypto_aead_reqtfm(req);
	struct cryptd_queue *queue = cryptd_get_queue(crypto_aead_tfm(tfm));

	rctx->complete = req->base.complete;
	req->base.complete = compl;
	return cryptd_enqueue_request(queue, &req->base);
}
