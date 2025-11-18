static int qeth_setadpparms_set_access_ctrl(struct qeth_card *card,
		enum qeth_ipa_isolation_modes isolation, int fallback)
{
	int rc;
	struct qeth_cmd_buffer *iob;
	struct qeth_ipa_cmd *cmd;
	struct qeth_set_access_ctrl *access_ctrl_req;

	QETH_CARD_TEXT(card, 4, "setacctl");

	QETH_DBF_TEXT_(SETUP, 2, "setacctl");
	QETH_DBF_TEXT_(SETUP, 2, "%s", card->gdev->dev.kobj.name);

	iob = qeth_get_adapter_cmd(card, IPA_SETADP_SET_ACCESS_CONTROL,
				   sizeof(struct qeth_ipacmd_setadpparms_hdr) +
				   sizeof(struct qeth_set_access_ctrl));
	cmd = (struct qeth_ipa_cmd *)(iob->data+IPA_PDU_HEADER_SIZE);
	access_ctrl_req = &cmd->data.setadapterparms.data.set_access_ctrl;
	access_ctrl_req->subcmd_code = isolation;

	rc = qeth_send_ipa_cmd(card, iob, qeth_setadpparms_set_access_ctrl_cb,
			       &fallback);
	QETH_DBF_TEXT_(SETUP, 2, "rc=%d", rc);
	return rc;
}
