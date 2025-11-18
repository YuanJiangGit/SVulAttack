void udp_destroy_sock(struct sock *sk)
{
	bool slow = lock_sock_fast(sk);
	udp_flush_pending_frames(sk);
	unlock_sock_fast(sk, slow);
}
