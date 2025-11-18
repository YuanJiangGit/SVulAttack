static int vmci_transport_notify_poll_in(
	struct vsock_sock *vsk,
	size_t target,
	bool *data_ready_now)
{
	return vmci_trans(vsk)->notify_ops->poll_in(
			&vsk->sk, target, data_ready_now);
}
