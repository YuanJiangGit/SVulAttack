inline void comps_rtree_pair_destroy_v(void * pair) {
    free(((COMPS_RTreePair *)pair)->key);
    free(pair);
}
