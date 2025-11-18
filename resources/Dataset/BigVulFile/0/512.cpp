static void pdf_run_c(fz_context *ctx, pdf_processor *proc, float x1, float y1, float x2, float y2, float x3, float y3)
{
	pdf_run_processor *pr = (pdf_run_processor *)proc;
	fz_curveto(ctx, pr->path, x1, y1, x2, y2, x3, y3);
}
