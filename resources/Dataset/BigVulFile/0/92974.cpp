process_polygon2(STREAM s, POLYGON2_ORDER * os, uint32 present, RD_BOOL delta)
{
	int index, data, next;
	uint8 flags = 0;
	RD_POINT *points;
	BRUSH brush;

	if (present & 0x0001)
		rdp_in_coord(s, &os->x, delta);

	if (present & 0x0002)
		rdp_in_coord(s, &os->y, delta);

	if (present & 0x0004)
		in_uint8(s, os->opcode);

	if (present & 0x0008)
		in_uint8(s, os->fillmode);

	if (present & 0x0010)
		rdp_in_colour(s, &os->bgcolour);

	if (present & 0x0020)
		rdp_in_colour(s, &os->fgcolour);

	rdp_parse_brush(s, &os->brush, present >> 6);

	if (present & 0x0800)
		in_uint8(s, os->npoints);

	if (present & 0x1000)
	{
		in_uint8(s, os->datasize);
		in_uint8a(s, os->data, os->datasize);
	}

	logger(Graphics, Debug,
	       "process_polygon2(), x=%d, y=%d, op=0x%x, fm=%d, bs=%d, bg=0x%x, fg=0x%x, n=%d, sz=%d)",
	       os->x, os->y, os->opcode, os->fillmode, os->brush.style, os->bgcolour, os->fgcolour,
	       os->npoints, os->datasize);

	if (os->opcode < 0x01 || os->opcode > 0x10)
	{
		logger(Graphics, Error, "process_polygon2(), bad ROP2 0x%x", os->opcode);
		return;
	}

	setup_brush(&brush, &os->brush);

	points = (RD_POINT *) xmalloc((os->npoints + 1) * sizeof(RD_POINT));
	memset(points, 0, (os->npoints + 1) * sizeof(RD_POINT));

	points[0].x = os->x;
	points[0].y = os->y;

	index = 0;
	data = ((os->npoints - 1) / 4) + 1;
	for (next = 1; (next <= os->npoints) && (next < 256) && (data < os->datasize); next++)
	{
		if ((next - 1) % 4 == 0)
			flags = os->data[index++];

		if (~flags & 0x80)
			points[next].x = parse_delta(os->data, &data);

		if (~flags & 0x40)
			points[next].y = parse_delta(os->data, &data);

		flags <<= 2;
	}

	if (next - 1 == os->npoints)
		ui_polygon(os->opcode - 1, os->fillmode, points, os->npoints + 1,
			   &brush, os->bgcolour, os->fgcolour);
	else
		logger(Graphics, Error, "process_polygon2(), polygon parse error");

	xfree(points);
}
