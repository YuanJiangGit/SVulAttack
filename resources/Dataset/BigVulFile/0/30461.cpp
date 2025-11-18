static int send_msg(struct kiocb *iocb, struct socket *sock,
		    struct msghdr *m, size_t total_len)
{
	struct sock *sk = sock->sk;
	struct tipc_port *tport = tipc_sk_port(sk);
	struct sockaddr_tipc *dest = (struct sockaddr_tipc *)m->msg_name;
	int needs_conn;
	long timeout_val;
	int res = -EINVAL;

	if (unlikely(!dest))
		return -EDESTADDRREQ;
	if (unlikely((m->msg_namelen < sizeof(*dest)) ||
		     (dest->family != AF_TIPC)))
		return -EINVAL;
	if (total_len > TIPC_MAX_USER_MSG_SIZE)
		return -EMSGSIZE;

	if (iocb)
		lock_sock(sk);

	needs_conn = (sock->state != SS_READY);
	if (unlikely(needs_conn)) {
		if (sock->state == SS_LISTENING) {
			res = -EPIPE;
			goto exit;
		}
		if (sock->state != SS_UNCONNECTED) {
			res = -EISCONN;
			goto exit;
		}
		if ((tport->published) ||
		    ((sock->type == SOCK_STREAM) && (total_len != 0))) {
			res = -EOPNOTSUPP;
			goto exit;
		}
		if (dest->addrtype == TIPC_ADDR_NAME) {
			tport->conn_type = dest->addr.name.name.type;
			tport->conn_instance = dest->addr.name.name.instance;
		}

		/* Abort any pending connection attempts (very unlikely) */
		reject_rx_queue(sk);
	}

	timeout_val = sock_sndtimeo(sk, m->msg_flags & MSG_DONTWAIT);

	do {
		if (dest->addrtype == TIPC_ADDR_NAME) {
			res = dest_name_check(dest, m);
			if (res)
				break;
			res = tipc_send2name(tport->ref,
					     &dest->addr.name.name,
					     dest->addr.name.domain,
					     m->msg_iovlen,
					     m->msg_iov,
					     total_len);
		} else if (dest->addrtype == TIPC_ADDR_ID) {
			res = tipc_send2port(tport->ref,
					     &dest->addr.id,
					     m->msg_iovlen,
					     m->msg_iov,
					     total_len);
		} else if (dest->addrtype == TIPC_ADDR_MCAST) {
			if (needs_conn) {
				res = -EOPNOTSUPP;
				break;
			}
			res = dest_name_check(dest, m);
			if (res)
				break;
			res = tipc_multicast(tport->ref,
					     &dest->addr.nameseq,
					     m->msg_iovlen,
					     m->msg_iov,
					     total_len);
		}
		if (likely(res != -ELINKCONG)) {
			if (needs_conn && (res >= 0))
				sock->state = SS_CONNECTING;
			break;
		}
		if (timeout_val <= 0L) {
			res = timeout_val ? timeout_val : -EWOULDBLOCK;
			break;
		}
		release_sock(sk);
		timeout_val = wait_event_interruptible_timeout(*sk_sleep(sk),
					       !tport->congested, timeout_val);
		lock_sock(sk);
	} while (1);

exit:
	if (iocb)
		release_sock(sk);
	return res;
}
