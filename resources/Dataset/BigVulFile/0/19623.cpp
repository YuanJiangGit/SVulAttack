static __inline__ void fq_kill(struct nf_ct_frag6_queue *fq)
{
	inet_frag_kill(&fq->q, &nf_frags);
}
