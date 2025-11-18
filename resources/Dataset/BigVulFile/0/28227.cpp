av_cold void ff_h264_free_context(H264Context *h)
{
    int i;

    free_tables(h, 1); // FIXME cleanup init stuff perhaps

    for (i = 0; i < MAX_SPS_COUNT; i++)
        av_freep(h->sps_buffers + i);

    for (i = 0; i < MAX_PPS_COUNT; i++)
        av_freep(h->pps_buffers + i);
}
