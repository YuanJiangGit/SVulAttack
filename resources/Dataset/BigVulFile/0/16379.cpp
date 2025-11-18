static int use_compress( const char * /* method */, const char *path )
{
	return attr_list_has_file( ATTR_COMPRESS_FILES, path );
}
