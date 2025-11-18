void CSoundFile::FreeSample(LPVOID p)
{
	if (p)
	{
		GlobalFreePtr(((LPSTR)p)-16);
	}
}
