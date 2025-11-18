bitmap_decompress4(uint8 * output, int width, int height, uint8 * input, int size)
{
	int code;
	int bytes_pro;
	int total_pro;

	code = CVAL(input);
	if (code != 0x10)
	{
		return False;
	}
	total_pro = 1;
	bytes_pro = process_plane(input, width, height, output + 3, size - total_pro);
	total_pro += bytes_pro;
	input += bytes_pro;
	bytes_pro = process_plane(input, width, height, output + 2, size - total_pro);
	total_pro += bytes_pro;
	input += bytes_pro;
	bytes_pro = process_plane(input, width, height, output + 1, size - total_pro);
	total_pro += bytes_pro;
	input += bytes_pro;
	bytes_pro = process_plane(input, width, height, output + 0, size - total_pro);
	total_pro += bytes_pro;
	return size == total_pro;
}
