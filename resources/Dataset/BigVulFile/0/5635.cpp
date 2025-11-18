ask (char const *format, ...)
{
  static int ttyfd = -2;
  ssize_t r;
  va_list args;

  va_start (args, format);
  vfprintf (stdout, format, args);
  va_end (args);
  fflush (stdout);

  if (ttyfd == -2)
    {
      /* If standard output is not a tty, don't bother opening /dev/tty,
	 since it's unlikely that stdout will be seen by the tty user.
	 The isatty test also works around a bug in GNU Emacs 19.34 under Linux
	 which makes a call-process `patch' hang when it reads from /dev/tty.
	 POSIX.1-2001 XCU line 26599 requires that we read /dev/tty,
	 though.  */
      ttyfd = (posixly_correct || isatty (STDOUT_FILENO)
	       ? open (TTY_DEVICE, O_RDONLY)
	       : -1);
    }

  if (ttyfd < 0)
    {
      /* No terminal at all -- default it.  */
      printf ("\n");
      buf[0] = '\n';
      buf[1] = '\0';
    }
  else
    {
      size_t s = 0;
      while ((r = read (ttyfd, buf + s, bufsize - 1 - s)) == bufsize - 1 - s
	     && buf[bufsize - 2] != '\n')
	{
	  s = bufsize - 1;
	  bufsize *= 2;
	  buf = realloc (buf, bufsize);
	  if (!buf)
	    xalloc_die ();
	}
      if (r == 0)
	printf ("EOF\n");
      else if (r < 0)
	{
	  perror ("tty read");
	  fflush (stderr);
	  close (ttyfd);
	  ttyfd = -1;
	  r = 0;
	}
      buf[s + r] = '\0';
    }
}
