static int genl_bind(struct net *net, int group)
{
	struct genl_family *f;
	int err = -ENOENT;
	unsigned int id;

	down_read(&cb_lock);

	idr_for_each_entry(&genl_fam_idr, f, id) {
		if (group >= f->mcgrp_offset &&
		    group < f->mcgrp_offset + f->n_mcgrps) {
			int fam_grp = group - f->mcgrp_offset;

			if (!f->netnsok && net != &init_net)
				err = -ENOENT;
			else if (f->mcast_bind)
				err = f->mcast_bind(net, fam_grp);
			else
				err = 0;
			break;
		}
	}
	up_read(&cb_lock);

	return err;
}
