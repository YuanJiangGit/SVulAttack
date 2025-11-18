static void ipa_poly_line(wmfAPI * API, wmfPolyLine_t * polyline)
{
  if (polyline->count <= 2)
    return;

  if (TO_DRAW(polyline))
    {
      int
        point;

      /* Save graphic wand */
      (void) PushDrawingWand(WmfDrawingWand);

      util_set_pen(API, polyline->dc);

      DrawPathStart(WmfDrawingWand);
      DrawPathMoveToAbsolute(WmfDrawingWand,
                             XC(polyline->pt[0].x),
                             YC(polyline->pt[0].y));
      for (point = 1; point < polyline->count; point++)
        {
          DrawPathLineToAbsolute(WmfDrawingWand,
                                 XC(polyline->pt[point].x),
                                 YC(polyline->pt[point].y));
        }
      DrawPathFinish(WmfDrawingWand);

      /* Restore graphic wand */
      (void) PopDrawingWand(WmfDrawingWand);
    }
}
