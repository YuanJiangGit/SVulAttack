static void iwbmp_write_file_header(struct iwbmpwcontext *wctx)
{
	iw_byte fileheader[14];

	if(!wctx->include_file_header) return;

	iw_zeromem(fileheader,sizeof(fileheader));
	fileheader[0] = 66; // 'B'
	fileheader[1] = 77; // 'M'

	iw_set_ui32le(&fileheader[ 2], (unsigned int)(14+wctx->header_size+
		wctx->bitfields_size+wctx->palsize+wctx->unc_bitssize)); // bfSize
	iw_set_ui32le(&fileheader[10],(unsigned int)(14+wctx->header_size+
		wctx->bitfields_size+wctx->palsize)); // bfOffBits
	iwbmp_write(wctx,fileheader,14);
}
