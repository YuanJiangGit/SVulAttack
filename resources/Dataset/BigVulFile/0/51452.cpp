void gdImageSetStyle (gdImagePtr im, int *style, int noOfPixels)
{
	if (im->style) {
		gdFree(im->style);
	}
	im->style = (int *) gdMalloc(sizeof(int) * noOfPixels);
	memcpy(im->style, style, sizeof(int) * noOfPixels);
	im->styleLength = noOfPixels;
	im->stylePos = 0;
}
