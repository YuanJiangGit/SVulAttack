  psh_glyph_find_blue_points( PSH_Blues  blues,
                              PSH_Glyph  glyph )
  {
    PSH_Blue_Table  table;
    PSH_Blue_Zone   zone;
    FT_UInt         glyph_count = glyph->num_points;
    FT_UInt         blue_count;
    PSH_Point       point = glyph->points;


    for ( ; glyph_count > 0; glyph_count--, point++ )
    {
      FT_Pos  y;


      /* check tangents */
      if ( !PSH_DIR_COMPARE( point->dir_in,  PSH_DIR_HORIZONTAL ) &&
           !PSH_DIR_COMPARE( point->dir_out, PSH_DIR_HORIZONTAL ) )
        continue;

      /* skip strong points */
      if ( psh_point_is_strong( point ) )
        continue;

      y = point->org_u;

      /* look up top zones */
      table      = &blues->normal_top;
      blue_count = table->count;
      zone       = table->zones;

      for ( ; blue_count > 0; blue_count--, zone++ )
      {
        FT_Pos  delta = y - zone->org_bottom;


        if ( delta < -blues->blue_fuzz )
          break;

        if ( y <= zone->org_top + blues->blue_fuzz )
          if ( blues->no_overshoots || delta <= blues->blue_threshold )
          {
            point->cur_u = zone->cur_bottom;
            psh_point_set_strong( point );
            psh_point_set_fitted( point );
          }
      }

      /* look up bottom zones */
      table      = &blues->normal_bottom;
      blue_count = table->count;
      zone       = table->zones + blue_count - 1;

      for ( ; blue_count > 0; blue_count--, zone-- )
      {
        FT_Pos  delta = zone->org_top - y;


        if ( delta < -blues->blue_fuzz )
          break;

        if ( y >= zone->org_bottom - blues->blue_fuzz )
          if ( blues->no_overshoots || delta < blues->blue_threshold )
          {
            point->cur_u = zone->cur_top;
            psh_point_set_strong( point );
            psh_point_set_fitted( point );
          }
      }
    }
  }
