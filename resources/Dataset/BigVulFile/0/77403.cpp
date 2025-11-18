ofproto_wait(struct ofproto *p)
{
    p->ofproto_class->wait(p);
    if (p->ofproto_class->port_poll_wait) {
        p->ofproto_class->port_poll_wait(p);
    }
    seq_wait(connectivity_seq_get(), p->change_seq);
    connmgr_wait(p->connmgr);
}
