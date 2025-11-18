static inline void phar_write_32(char buffer[4], php_uint32 value)
{
	buffer[3] = (unsigned char) ((value & 0xff000000) >> 24);
	buffer[2] = (unsigned char) ((value & 0xff0000) >> 16);
	buffer[1] = (unsigned char) ((value & 0xff00) >> 8);
	buffer[0] = (unsigned char) (value & 0xff);
}
