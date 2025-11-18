static struct key *request_master_key(struct encrypted_key_payload *epayload,
				      const u8 **master_key, size_t *master_keylen)
{
	struct key *mkey = ERR_PTR(-EINVAL);

	if (!strncmp(epayload->master_desc, KEY_TRUSTED_PREFIX,
		     KEY_TRUSTED_PREFIX_LEN)) {
		mkey = request_trusted_key(epayload->master_desc +
					   KEY_TRUSTED_PREFIX_LEN,
					   master_key, master_keylen);
	} else if (!strncmp(epayload->master_desc, KEY_USER_PREFIX,
			    KEY_USER_PREFIX_LEN)) {
		mkey = request_user_key(epayload->master_desc +
					KEY_USER_PREFIX_LEN,
					master_key, master_keylen);
	} else
		goto out;

	if (IS_ERR(mkey)) {
		int ret = PTR_ERR(mkey);

		if (ret == -ENOTSUPP)
			pr_info("encrypted_key: key %s not supported",
				epayload->master_desc);
		else
			pr_info("encrypted_key: key %s not found",
				epayload->master_desc);
		goto out;
	}

	dump_master_key(*master_key, *master_keylen);
out:
	return mkey;
}
