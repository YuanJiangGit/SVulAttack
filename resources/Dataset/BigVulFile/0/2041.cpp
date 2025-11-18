static void null_terminate_endl(char *source)
{
	char *newline = strchr(source, '\n');
	if (newline)
		*newline = '\0';
}
