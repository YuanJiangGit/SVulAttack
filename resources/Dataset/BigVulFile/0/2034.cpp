drop_capabilities(int parent)
{
	int rc, ncaps;
	cap_t caps;
	cap_value_t cap_list[3];

	rc = prune_bounding_set();
	if (rc)
		return rc;

	caps = cap_get_proc();
	if (caps == NULL) {
		fprintf(stderr, "Unable to get current capability set: %s\n",
			strerror(errno));
		return EX_SYSERR;
	}

	if (cap_clear(caps) == -1) {
		fprintf(stderr, "Unable to clear capability set: %s\n",
			strerror(errno));
		rc = EX_SYSERR;
		goto free_caps;
	}

	if (parent || getuid() == 0) {
		ncaps = 1;
		cap_list[0] = CAP_DAC_READ_SEARCH;
		if (parent) {
			cap_list[1] = CAP_DAC_OVERRIDE;
			cap_list[2] = CAP_SYS_ADMIN;
			ncaps += 2;
		}
		if (cap_set_flag(caps, CAP_PERMITTED, ncaps, cap_list, CAP_SET) == -1) {
			fprintf(stderr, "Unable to set permitted capabilities: %s\n",
				strerror(errno));
			rc = EX_SYSERR;
			goto free_caps;
		}
		if (parent) {
			cap_list[0] = CAP_SYS_ADMIN;
			if (cap_set_flag(caps, CAP_EFFECTIVE, 1, cap_list, CAP_SET) == -1) {
				fprintf(stderr, "Unable to set effective capabilities: %s\n",
					strerror(errno));
				rc = EX_SYSERR;
				goto free_caps;
			}
		}
	}

	if (cap_set_proc(caps) != 0) {
		fprintf(stderr, "Unable to set current process capabilities: %s\n",
			strerror(errno));
		rc = EX_SYSERR;
	}
free_caps:
	cap_free(caps);
	return rc;
}
