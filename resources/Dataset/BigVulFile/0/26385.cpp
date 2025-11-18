static int __init nowatchdog_setup(char *str)
{
	watchdog_enabled = 0;
	return 1;
}
