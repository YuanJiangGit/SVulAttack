static unsigned int ssh_tty_parse_specchar(char *s)
{
    unsigned int ret;
    if (*s) {
	char *next = NULL;
	ret = ctrlparse(s, &next);
	if (!next) ret = s[0];
    } else {
	ret = 255; /* special value meaning "don't set" */
    }
    return ret;
}
