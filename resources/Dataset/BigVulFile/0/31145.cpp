static void fb_rotate_logo_ccw(const u8 *in, u8 *out, u32 width, u32 height)
{
	int i, j, w = width - 1;

	for (i = 0; i < height; i++)
		for (j = 0; j < width; j++)
			out[height * (w - j) + i] = *in++;
}
