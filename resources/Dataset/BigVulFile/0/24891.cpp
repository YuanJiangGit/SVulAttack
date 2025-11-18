static int __init setup_slub_min_objects(char *str)
{
	get_option(&str, &slub_min_objects);

	return 1;
}
