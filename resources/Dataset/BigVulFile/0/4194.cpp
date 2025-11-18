  tt_cmap6_char_next( TT_CMap     cmap,
                      FT_UInt32  *pchar_code )
  {
    FT_Byte*   table     = cmap->data;
    FT_UInt32  result    = 0;
    FT_UInt32  char_code = *pchar_code + 1;
    FT_UInt    gindex    = 0;

    FT_Byte*   p         = table + 6;
    FT_UInt    start     = TT_NEXT_USHORT( p );
    FT_UInt    count     = TT_NEXT_USHORT( p );
    FT_UInt    idx;


    if ( char_code >= 0x10000UL )
      goto Exit;

    if ( char_code < start )
      char_code = start;

    idx = (FT_UInt)( char_code - start );
    p  += 2 * idx;

    for ( ; idx < count; idx++ )
    {
      gindex = TT_NEXT_USHORT( p );
      if ( gindex != 0 )
      {
        result = char_code;
        break;
      }
      char_code++;
    }

  Exit:
    *pchar_code = result;
    return gindex;
  }
