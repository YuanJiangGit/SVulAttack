TIFFReadDirEntryCheckRangeLongSlong8(int64 value)
{
	if ((value < 0) || (value > (int64) TIFF_UINT32_MAX))
		return(TIFFReadDirEntryErrRange);
	else
		return(TIFFReadDirEntryErrOk);
}
