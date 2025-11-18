stf_status ikev2_decrypt_msg(struct msg_digest *md,
			     enum phase1_role init)
{
	struct state *st = md->st;
	unsigned char *encend;
	pb_stream     *e_pbs;
	unsigned int np;
	unsigned char *iv;
	chunk_t       *cipherkey, *authkey;
	unsigned char *authstart;
	struct state *pst = st;

	if (st->st_clonedfrom != 0)
		pst = state_with_serialno(st->st_clonedfrom);

	if (init == INITIATOR) {
		cipherkey = &pst->st_skey_er;
		authkey   = &pst->st_skey_ar;
	} else {
		cipherkey = &pst->st_skey_ei;
		authkey   = &pst->st_skey_ai;
	}

	e_pbs = &md->chain[ISAKMP_NEXT_v2E]->pbs;
	np    = md->chain[ISAKMP_NEXT_v2E]->payload.generic.isag_np;

	authstart = md->packet_pbs.start;
	iv     = e_pbs->cur;
	encend = e_pbs->roof - pst->st_oakley.integ_hasher->hash_integ_len;

	/* start by checking authenticator */
	{
		unsigned char  *b12 = alloca(
			pst->st_oakley.integ_hasher->hash_digest_len);
		struct hmac_ctx ctx;

		hmac_init_chunk(&ctx, pst->st_oakley.integ_hasher, *authkey);
		hmac_update(&ctx, authstart, encend - authstart);
		hmac_final(b12, &ctx);

		DBG(DBG_PARSING, {
			    DBG_dump("data being hmac:", authstart, encend -
				     authstart);
			    DBG_dump("R2 calculated auth:", b12,
				     pst->st_oakley.integ_hasher->
				     hash_integ_len);
			    DBG_dump("R2  provided  auth:", encend,
				     pst->st_oakley.integ_hasher->
				     hash_integ_len);
		    });

		/* compare first 96 bits == 12 bytes */
		/* It is not always 96 bytes, it depends upon which integ algo is used*/
		if (memcmp(b12, encend,
			   pst->st_oakley.integ_hasher->hash_integ_len) != 0) {
			libreswan_log("R2 failed to match authenticator");
			return STF_FAIL;
		}
	}

	DBG(DBG_PARSING, DBG_log("authenticator matched"));

	/* decrypt */
	{
		size_t blocksize = pst->st_oakley.encrypter->enc_blocksize;
		unsigned char *encstart  = iv + blocksize;
		unsigned int enclen    = encend - encstart;
		unsigned int padlen;

		DBG(DBG_CRYPT,
		    DBG_dump("data before decryption:", encstart, enclen));

		/* now, decrypt */
		(pst->st_oakley.encrypter->do_crypt)(encstart,
						     enclen,
						     cipherkey->ptr,
						     cipherkey->len,
						     iv, FALSE);

		padlen = encstart[enclen - 1];
		encend = encend - padlen + 1;

		if (encend < encstart) {
			libreswan_log("invalid pad length: %u", padlen);
			return STF_FAIL;
		}

		DBG(DBG_CRYPT, {
			    DBG_dump("decrypted payload:", encstart, enclen);
			    DBG_log("striping %u bytes as pad", padlen + 1);
		    });

		init_pbs(&md->clr_pbs, encstart, enclen - (padlen + 1),
			 "cleartext");
	}

	{
		stf_status ret;
		ret =
			ikev2_process_payloads(md, &md->clr_pbs, st->st_state,
					       np);
		if (ret != STF_OK)
			return ret;
	}

	return STF_OK;
}
