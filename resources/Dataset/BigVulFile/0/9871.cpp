int conn_si_send_proxy(struct connection *conn, unsigned int flag)
{
	/* we might have been called just after an asynchronous shutw */
	if (conn->flags & CO_FL_SOCK_WR_SH)
		goto out_error;

	if (!conn_ctrl_ready(conn))
		goto out_error;

	if (!fd_send_ready(conn->t.sock.fd))
		goto out_wait;

	/* If we have a PROXY line to send, we'll use this to validate the
	 * connection, in which case the connection is validated only once
	 * we've sent the whole proxy line. Otherwise we use connect().
	 */
	while (conn->send_proxy_ofs) {
		int ret;

		/* The target server expects a PROXY line to be sent first.
		 * If the send_proxy_ofs is negative, it corresponds to the
		 * offset to start sending from then end of the proxy string
		 * (which is recomputed every time since it's constant). If
		 * it is positive, it means we have to send from the start.
		 * We can only send a "normal" PROXY line when the connection
		 * is attached to a stream interface. Otherwise we can only
		 * send a LOCAL line (eg: for use with health checks).
		 */
		if (conn->data == &si_conn_cb) {
			struct stream_interface *si = conn->owner;
			struct connection *remote = objt_conn(si->ob->prod->end);
			ret = make_proxy_line(trash.str, trash.size, objt_server(conn->target), remote);
		}
		else {
			/* The target server expects a LOCAL line to be sent first. Retrieving
			 * local or remote addresses may fail until the connection is established.
			 */
			conn_get_from_addr(conn);
			if (!(conn->flags & CO_FL_ADDR_FROM_SET))
				goto out_wait;

			conn_get_to_addr(conn);
			if (!(conn->flags & CO_FL_ADDR_TO_SET))
				goto out_wait;

			ret = make_proxy_line(trash.str, trash.size, objt_server(conn->target), conn);
		}

		if (!ret)
			goto out_error;

		if (conn->send_proxy_ofs > 0)
			conn->send_proxy_ofs = -ret; /* first call */

		/* we have to send trash from (ret+sp for -sp bytes). If the
		 * data layer has a pending write, we'll also set MSG_MORE.
		 */
		ret = send(conn->t.sock.fd, trash.str + ret + conn->send_proxy_ofs, -conn->send_proxy_ofs,
			   (conn->flags & CO_FL_DATA_WR_ENA) ? MSG_MORE : 0);

		if (ret == 0)
			goto out_wait;

		if (ret < 0) {
			if (errno == EAGAIN || errno == ENOTCONN)
				goto out_wait;
			if (errno == EINTR)
				continue;
			conn->flags |= CO_FL_SOCK_RD_SH | CO_FL_SOCK_WR_SH;
			goto out_error;
		}

		conn->send_proxy_ofs += ret; /* becomes zero once complete */
		if (conn->send_proxy_ofs != 0)
			goto out_wait;

		/* OK we've sent the whole line, we're connected */
		break;
	}

	/* The connection is ready now, simply return and let the connection
	 * handler notify upper layers if needed.
	 */
	if (conn->flags & CO_FL_WAIT_L4_CONN)
		conn->flags &= ~CO_FL_WAIT_L4_CONN;
	conn->flags &= ~flag;
	return 1;

 out_error:
	/* Write error on the file descriptor */
	conn->flags |= CO_FL_ERROR;
	return 0;

 out_wait:
	__conn_sock_stop_recv(conn);
	fd_cant_send(conn->t.sock.fd);
	return 0;
}
