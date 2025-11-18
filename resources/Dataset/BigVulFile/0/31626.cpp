sctp_disposition_t sctp_sf_operr_notify(struct net *net,
					const struct sctp_endpoint *ep,
					const struct sctp_association *asoc,
					const sctp_subtype_t type,
					void *arg,
					sctp_cmd_seq_t *commands)
{
	struct sctp_chunk *chunk = arg;
	sctp_errhdr_t *err;

	if (!sctp_vtag_verify(chunk, asoc))
		return sctp_sf_pdiscard(net, ep, asoc, type, arg, commands);

	/* Make sure that the ERROR chunk has a valid length. */
	if (!sctp_chunk_length_valid(chunk, sizeof(sctp_operr_chunk_t)))
		return sctp_sf_violation_chunklen(net, ep, asoc, type, arg,
						  commands);
	sctp_walk_errors(err, chunk->chunk_hdr);
	if ((void *)err != (void *)chunk->chunk_end)
		return sctp_sf_violation_paramlen(net, ep, asoc, type, arg,
						  (void *)err, commands);

	sctp_add_cmd_sf(commands, SCTP_CMD_PROCESS_OPERR,
			SCTP_CHUNK(chunk));

	return SCTP_DISPOSITION_CONSUME;
}
