dist_lb(PG_FUNCTION_ARGS)
{
#ifdef NOT_USED
	LINE	   *line = PG_GETARG_LINE_P(0);
	BOX		   *box = PG_GETARG_BOX_P(1);
#endif

	/* need to think about this one for a while */
	ereport(ERROR,
			(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
			 errmsg("function \"dist_lb\" not implemented")));

	PG_RETURN_NULL();
}
