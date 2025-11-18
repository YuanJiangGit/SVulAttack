poly_right(PG_FUNCTION_ARGS)
{
	POLYGON    *polya = PG_GETARG_POLYGON_P(0);
	POLYGON    *polyb = PG_GETARG_POLYGON_P(1);
	bool		result;

	result = polya->boundbox.low.x > polyb->boundbox.high.x;

	/*
	 * Avoid leaking memory for toasted inputs ... needed for rtree indexes
	 */
	PG_FREE_IF_COPY(polya, 0);
	PG_FREE_IF_COPY(polyb, 1);

	PG_RETURN_BOOL(result);
}
