authentic_pin_get_policy (struct sc_card *card, struct sc_pin_cmd_data *data)
{
	struct sc_context *ctx = card->ctx;
	struct sc_apdu apdu;
	unsigned char rbuf[0x100];
	int ii, rv;

	LOG_FUNC_CALLED(ctx);
	sc_log(ctx, "get PIN(type:%X,ref:%X,tries-left:%i)", data->pin_type, data->pin_reference, data->pin1.tries_left);

	sc_format_apdu(card, &apdu, SC_APDU_CASE_2_SHORT, 0xCA, 0x5F, data->pin_reference);
	for (ii=0;ii<2;ii++)   {
		apdu.le = sizeof(rbuf);
		apdu.resp = rbuf;
		apdu.resplen = sizeof(rbuf);

		rv = sc_transmit_apdu(card, &apdu);
		LOG_TEST_RET(ctx, rv, "APDU transmit failed");
		rv = sc_check_sw(card, apdu.sw1, apdu.sw2);

		if (rv != SC_ERROR_CLASS_NOT_SUPPORTED)
			break;

		apdu.cla = 0x80;
	}
        LOG_TEST_RET(ctx, rv, "'GET DATA' error");

	data->pin1.tries_left = -1;

	rv = authentic_parse_credential_data(ctx, data, apdu.resp, apdu.resplen);
        LOG_TEST_RET(ctx, rv, "Cannot parse credential data");

	data->pin1.encoding = SC_PIN_ENCODING_ASCII;
	data->pin1.offset = 5;
	data->pin1.pad_char = 0xFF;
	data->pin1.pad_length = data->pin1.max_length;
	data->pin1.logged_in = SC_PIN_STATE_UNKNOWN;

	data->flags |= SC_PIN_CMD_NEED_PADDING;

	sc_log(ctx,
	       "PIN policy: size max/min/pad %"SC_FORMAT_LEN_SIZE_T"u/%"SC_FORMAT_LEN_SIZE_T"u/%"SC_FORMAT_LEN_SIZE_T"u, tries max/left %i/%i",
	       data->pin1.max_length, data->pin1.min_length,
	       data->pin1.pad_length, data->pin1.max_tries,
	       data->pin1.tries_left);

	LOG_FUNC_RETURN(ctx, rv);
}
