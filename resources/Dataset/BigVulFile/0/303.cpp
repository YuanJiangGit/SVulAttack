static void cmyk_to_rgb(fz_context *ctx, const fz_colorspace *cs, const float *cmyk, float *rgb)
{
#ifdef SLOWCMYK /* from poppler */
	float c = cmyk[0], m = cmyk[1], y = cmyk[2], k = cmyk[3];
	float r, g, b, x;
	float cm = c * m;
	float c1m = m - cm;
	float cm1 = c - cm;
	float c1m1 = 1 - m - cm1;
	float c1m1y = c1m1 * y;
	float c1m1y1 = c1m1 - c1m1y;
	float c1my = c1m * y;
	float c1my1 = c1m - c1my;
	float cm1y = cm1 * y;
	float cm1y1 = cm1 - cm1y;
	float cmy = cm * y;
	float cmy1 = cm - cmy;

	/* this is a matrix multiplication, unrolled for performance */
	x = c1m1y1 * k;		/* 0 0 0 1 */
	r = g = b = c1m1y1 - x;	/* 0 0 0 0 */
	r += 0.1373f * x;
	g += 0.1216f * x;
	b += 0.1255f * x;

	x = c1m1y * k;		/* 0 0 1 1 */
	r += 0.1098f * x;
	g += 0.1020f * x;
	x = c1m1y - x;		/* 0 0 1 0 */
	r += x;
	g += 0.9490f * x;

	x = c1my1 * k;		/* 0 1 0 1 */
	r += 0.1412f * x;
	x = c1my1 - x;		/* 0 1 0 0 */
	r += 0.9255f * x;
	b += 0.5490f * x;

	x = c1my * k;		/* 0 1 1 1 */
	r += 0.1333f * x;
	x = c1my - x;		/* 0 1 1 0 */
	r += 0.9294f * x;
	g += 0.1098f * x;
	b += 0.1412f * x;

	x = cm1y1 * k;		/* 1 0 0 1 */
	g += 0.0588f * x;
	b += 0.1412f * x;
	x = cm1y1 - x;		/* 1 0 0 0 */
	g += 0.6784f * x;
	b += 0.9373f * x;

	x = cm1y * k;		/* 1 0 1 1 */
	g += 0.0745f * x;
	x = cm1y - x;		/* 1 0 1 0 */
	g += 0.6510f * x;
	b += 0.3137f * x;

	x = cmy1 * k;		/* 1 1 0 1 */
	b += 0.0078f * x;
	x = cmy1 - x;		/* 1 1 0 0 */
	r += 0.1804f * x;
	g += 0.1922f * x;
	b += 0.5725f * x;

	x = cmy * (1-k);	/* 1 1 1 0 */
	r += 0.2118f * x;
	g += 0.2119f * x;
	b += 0.2235f * x;
	rgb[0] = fz_clamp(r, 0, 1);
	rgb[1] = fz_clamp(g, 0, 1);
	rgb[2] = fz_clamp(b, 0, 1);
#else
	rgb[0] = 1 - fz_min(1, cmyk[0] + cmyk[3]);
	rgb[1] = 1 - fz_min(1, cmyk[1] + cmyk[3]);
	rgb[2] = 1 - fz_min(1, cmyk[2] + cmyk[3]);
#endif
}
