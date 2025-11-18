get_match(
	char *s,
	struct masks *m
	)
{
	while (m->name) {
		if (strcmp(s, m->name) == 0)
			return m->mask;
		else
			m++;
	}
	return 0;
}
