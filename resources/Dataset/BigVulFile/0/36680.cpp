void *str_text(char *ptr)
{
	unsigned char *uptr;
	char *ret, *txt;

	if (ptr == NULL) {
		ret = strdup("(null)");

		if (unlikely(!ret))
			quithere(1, "Failed to malloc null");
	}

	uptr = (unsigned char *)ptr;

	ret = txt = malloc(strlen(ptr)*4+5); // Guaranteed >= needed
	if (unlikely(!txt))
		quithere(1, "Failed to malloc txt");

	do {
		if (*uptr < ' ' || *uptr > '~') {
			sprintf(txt, "0x%02x", *uptr);
			txt += 4;
		} else
			*(txt++) = *uptr;
	} while (*(uptr++));

	*txt = '\0';

	return ret;
}
