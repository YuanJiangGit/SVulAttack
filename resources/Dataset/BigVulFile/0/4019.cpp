GBool DCTStream::readJFIFMarker() {
  int length, i;
  char buf[5];
  int c;

  length = read16();
  length -= 2;
  if (length >= 5) {
    for (i = 0; i < 5; ++i) {
      if ((c = str->getChar()) == EOF) {
	error(errSyntaxError, getPos(), "Bad DCT APP0 marker");
	return gFalse;
      }
      buf[i] = c;
    }
    length -= 5;
    if (!memcmp(buf, "JFIF\0", 5)) {
      gotJFIFMarker = gTrue;
    }
  }
  while (length > 0) {
    if (str->getChar() == EOF) {
      error(errSyntaxError, getPos(), "Bad DCT APP0 marker");
      return gFalse;
    }
    --length;
  }
  return gTrue;
}
