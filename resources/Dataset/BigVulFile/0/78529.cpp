acl_to_ac_byte(struct sc_card *card, const struct sc_acl_entry *e)
{
	unsigned key_ref;

	if (e == NULL)
		return SC_ERROR_OBJECT_NOT_FOUND;

	key_ref = e->key_ref & ~OBERTHUR_PIN_LOCAL;

	switch (e->method) {
	case SC_AC_NONE:
		LOG_FUNC_RETURN(card->ctx, 0);

	case SC_AC_CHV:
		if (key_ref > 0 && key_ref < 6)
			LOG_FUNC_RETURN(card->ctx, (0x20 | key_ref));
		else
			LOG_FUNC_RETURN(card->ctx, SC_ERROR_INCORRECT_PARAMETERS);

	case SC_AC_PRO:
		if (((key_ref & 0xE0) != 0x60) || ((key_ref & 0x18) == 0))
			LOG_FUNC_RETURN(card->ctx, SC_ERROR_INCORRECT_PARAMETERS);
		else
			LOG_FUNC_RETURN(card->ctx, key_ref);

	case SC_AC_NEVER:
		return 0xff;
	}

	LOG_FUNC_RETURN(card->ctx, SC_ERROR_INCORRECT_PARAMETERS);
}
