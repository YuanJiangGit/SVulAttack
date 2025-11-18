static int jas_icclut8_output(jas_iccattrval_t *attrval, jas_stream_t *out)
{
	jas_icclut8_t *lut8 = &attrval->data.lut8;
	int i;
	int j;
	int n;
	lut8->clut = 0;
	lut8->intabs = 0;
	lut8->intabsbuf = 0;
	lut8->outtabs = 0;
	lut8->outtabsbuf = 0;
	if (jas_stream_putc(out, lut8->numinchans) == EOF ||
	  jas_stream_putc(out, lut8->numoutchans) == EOF ||
	  jas_stream_putc(out, lut8->clutlen) == EOF ||
	  jas_stream_putc(out, 0) == EOF)
		goto error;
	for (i = 0; i < 3; ++i) {
		for (j = 0; j < 3; ++j) {
			if (jas_iccputsint32(out, lut8->e[i][j]))
				goto error;
		}
	}
	if (jas_iccputuint16(out, lut8->numintabents) ||
	  jas_iccputuint16(out, lut8->numouttabents))
		goto error;
	n = lut8->numinchans * lut8->numintabents;
	for (i = 0; i < n; ++i) {
		if (jas_iccputuint8(out, lut8->intabsbuf[i]))
			goto error;
	}
	n = lut8->numoutchans * lut8->numouttabents;
	for (i = 0; i < n; ++i) {
		if (jas_iccputuint8(out, lut8->outtabsbuf[i]))
			goto error;
	}
	n = jas_iccpowi(lut8->clutlen, lut8->numinchans) * lut8->numoutchans;
	for (i = 0; i < n; ++i) {
		if (jas_iccputuint8(out, lut8->clut[i]))
			goto error;
	}
	return 0;
error:
	return -1;
}
