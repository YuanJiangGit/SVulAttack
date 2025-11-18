  FT_GetFilePath_From_Mac_ATS_Name( const char*  fontName,
                                    UInt8*       path,
                                    UInt32       maxPathSize,
                                    FT_Long*     face_index )
  {
    FSRef     ref;
    FT_Error  err;


    err = FT_GetFileRef_From_Mac_ATS_Name( fontName, &ref, face_index );
    if ( err )
      return err;

    if ( noErr != FSRefMakePath( &ref, path, maxPathSize ) )
      return FT_THROW( Unknown_File_Format );

    return FT_Err_Ok;
  }
