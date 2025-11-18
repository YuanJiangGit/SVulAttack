  bool CompareRowSegment(
      GLint x, GLint y, GLsizei width, const void* data) const {
    DCHECK(x + width <= width_ || width == 0);
    return memcmp(data, GetPixelAddress(x, y), width * bytes_per_pixel_) == 0;
  }
