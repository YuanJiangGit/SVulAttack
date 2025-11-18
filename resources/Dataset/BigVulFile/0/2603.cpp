json_each_text(PG_FUNCTION_ARGS)
{
	return each_worker(fcinfo, true);
}
