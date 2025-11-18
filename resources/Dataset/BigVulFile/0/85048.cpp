static CURLcode smtp_multi_statemach(struct connectdata *conn, bool *done)
{
  CURLcode result = CURLE_OK;
  struct smtp_conn *smtpc = &conn->proto.smtpc;

  if((conn->handler->flags & PROTOPT_SSL) && !smtpc->ssldone) {
    result = Curl_ssl_connect_nonblocking(conn, FIRSTSOCKET, &smtpc->ssldone);
    if(result || !smtpc->ssldone)
      return result;
  }

  result = Curl_pp_statemach(&smtpc->pp, FALSE);
  *done = (smtpc->state == SMTP_STOP) ? TRUE : FALSE;

  return result;
}
