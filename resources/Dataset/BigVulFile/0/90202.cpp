void rds_tcp_tune(struct socket *sock)
{
	struct sock *sk = sock->sk;
	struct net *net = sock_net(sk);
	struct rds_tcp_net *rtn = net_generic(net, rds_tcp_netid);

	rds_tcp_nonagle(sock);
	lock_sock(sk);
	if (rtn->sndbuf_size > 0) {
		sk->sk_sndbuf = rtn->sndbuf_size;
		sk->sk_userlocks |= SOCK_SNDBUF_LOCK;
	}
	if (rtn->rcvbuf_size > 0) {
		sk->sk_sndbuf = rtn->rcvbuf_size;
		sk->sk_userlocks |= SOCK_RCVBUF_LOCK;
	}
	release_sock(sk);
}
