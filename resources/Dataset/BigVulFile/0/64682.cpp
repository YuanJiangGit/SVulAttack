onig_region_set(OnigRegion* region, int at, int beg, int end)
{
  if (at < 0) return ONIGERR_INVALID_ARGUMENT;

  if (at >= region->allocated) {
    int r = onig_region_resize(region, at + 1);
    if (r < 0) return r;
  }
  
  region->beg[at] = beg;
  region->end[at] = end;
  return 0;
}
