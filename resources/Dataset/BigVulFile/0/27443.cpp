ipip6_tunnel_del_prl(struct ip_tunnel *t, struct ip_tunnel_prl *a)
{
	struct ip_tunnel_prl_entry *x, **p;
	int err = 0;

	spin_lock(&ipip6_prl_lock);

	if (a && a->addr != htonl(INADDR_ANY)) {
		for (p = &t->prl; *p; p = &(*p)->next) {
			if ((*p)->addr == a->addr) {
				x = *p;
				*p = x->next;
				call_rcu(&x->rcu_head, prl_entry_destroy_rcu);
				t->prl_count--;
				goto out;
			}
		}
		err = -ENXIO;
	} else {
		if (t->prl) {
			t->prl_count = 0;
			x = t->prl;
			call_rcu(&x->rcu_head, prl_list_destroy_rcu);
			t->prl = NULL;
		}
	}
out:
	spin_unlock(&ipip6_prl_lock);
	return err;
}
