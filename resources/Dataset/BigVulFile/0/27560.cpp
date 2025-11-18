static void opl3_bender(int dev, int voice, int value)
{
	if (voice < 0 || voice >= devc->nr_voice)
		return;

	bend_pitch(dev, voice, value - 8192);
}
