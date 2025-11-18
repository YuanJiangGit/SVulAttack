  FT_Stream_TryRead( FT_Stream  stream,
                     FT_Byte*   buffer,
                     FT_ULong   count )
  {
    FT_ULong  read_bytes = 0;


    if ( stream->pos >= stream->size )
      goto Exit;

    if ( stream->read )
      read_bytes = stream->read( stream, stream->pos, buffer, count );
    else
    {
      read_bytes = stream->size - stream->pos;
      if ( read_bytes > count )
        read_bytes = count;

      FT_MEM_COPY( buffer, stream->base + stream->pos, read_bytes );
    }

    stream->pos += read_bytes;

  Exit:
    return read_bytes;
  }
