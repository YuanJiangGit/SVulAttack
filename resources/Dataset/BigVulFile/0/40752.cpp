static int vsock_dgram_sendmsg(struct kiocb *kiocb, struct socket *sock,
			       struct msghdr *msg, size_t len)
{
	int err;
	struct sock *sk;
	struct vsock_sock *vsk;
	struct sockaddr_vm *remote_addr;

	if (msg->msg_flags & MSG_OOB)
		return -EOPNOTSUPP;

	/* For now, MSG_DONTWAIT is always assumed... */
	err = 0;
	sk = sock->sk;
	vsk = vsock_sk(sk);

	lock_sock(sk);

	err = vsock_auto_bind(vsk);
	if (err)
		goto out;


	/* If the provided message contains an address, use that.  Otherwise
	 * fall back on the socket's remote handle (if it has been connected).
	 */
	if (msg->msg_name &&
	    vsock_addr_cast(msg->msg_name, msg->msg_namelen,
			    &remote_addr) == 0) {
		/* Ensure this address is of the right type and is a valid
		 * destination.
		 */

		if (remote_addr->svm_cid == VMADDR_CID_ANY)
			remote_addr->svm_cid = transport->get_local_cid();

		if (!vsock_addr_bound(remote_addr)) {
			err = -EINVAL;
			goto out;
		}
	} else if (sock->state == SS_CONNECTED) {
		remote_addr = &vsk->remote_addr;

		if (remote_addr->svm_cid == VMADDR_CID_ANY)
			remote_addr->svm_cid = transport->get_local_cid();

		/* XXX Should connect() or this function ensure remote_addr is
		 * bound?
		 */
		if (!vsock_addr_bound(&vsk->remote_addr)) {
			err = -EINVAL;
			goto out;
		}
	} else {
		err = -EINVAL;
		goto out;
	}

	if (!transport->dgram_allow(remote_addr->svm_cid,
				    remote_addr->svm_port)) {
		err = -EINVAL;
		goto out;
	}

	err = transport->dgram_enqueue(vsk, remote_addr, msg->msg_iov, len);

out:
	release_sock(sk);
	return err;
}
