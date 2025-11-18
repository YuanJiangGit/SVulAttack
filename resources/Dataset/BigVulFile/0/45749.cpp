static int crypto_rfc4106_decrypt(struct aead_request *req)
{
	req = crypto_rfc4106_crypt(req);

	return crypto_aead_decrypt(req);
}
