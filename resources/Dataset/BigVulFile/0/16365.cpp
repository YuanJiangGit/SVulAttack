int pseudo_get_buffer_info( int *bytes_out, int *block_size_out, int *prefetch_bytes_out )
{
	int bytes=0, block_size=0;

	Shadow->getJobAd()->LookupInteger(ATTR_BUFFER_SIZE,bytes);
	Shadow->getJobAd()->LookupInteger(ATTR_BUFFER_BLOCK_SIZE,block_size);

	if( bytes<0 ) bytes = 0;
	if( block_size<0 ) block_size = 0;
	if( bytes<block_size ) block_size = bytes;

	*bytes_out = bytes;
	*block_size_out = block_size;
	*prefetch_bytes_out = 0;

	dprintf(D_SYSCALLS,"\tbuffer configuration is bytes=%d block_size=%d\n",bytes, block_size );

	return 0;
}
