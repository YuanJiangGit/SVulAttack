TIFFFetchSubjectDistance(TIFF* tif, TIFFDirEntry* dir)
{
	static const char module[] = "TIFFFetchSubjectDistance";
	enum TIFFReadDirEntryErr err;
	UInt64Aligned_t m;
    m.l=0;
	assert(sizeof(double)==8);
	assert(sizeof(uint64)==8);
	assert(sizeof(uint32)==4);
	if (dir->tdir_count!=1)
		err=TIFFReadDirEntryErrCount;
	else if (dir->tdir_type!=TIFF_RATIONAL)
		err=TIFFReadDirEntryErrType;
	else
	{
		if (!(tif->tif_flags&TIFF_BIGTIFF))
		{
			uint32 offset;
			offset=*(uint32*)(&dir->tdir_offset);
			if (tif->tif_flags&TIFF_SWAB)
				TIFFSwabLong(&offset);
			err=TIFFReadDirEntryData(tif,offset,8,m.i);
		}
		else
		{
			m.l=dir->tdir_offset.toff_long8;
			err=TIFFReadDirEntryErrOk;
		}
	}
	if (err==TIFFReadDirEntryErrOk)
	{
		double n;
		if (tif->tif_flags&TIFF_SWAB)
			TIFFSwabArrayOfLong(m.i,2);
		if (m.i[0]==0)
			n=0.0;
		else if (m.i[0]==0xFFFFFFFF)
			/*
			 * XXX: Numerator 0xFFFFFFFF means that we have infinite
			 * distance. Indicate that with a negative floating point
			 * SubjectDistance value.
			 */
			n=-1.0;
		else
			n=(double)m.i[0]/(double)m.i[1];
		return(TIFFSetField(tif,dir->tdir_tag,n));
	}
	else
	{
		TIFFReadDirEntryOutputErr(tif,err,module,"SubjectDistance",TRUE);
		return(0);
	}
}
