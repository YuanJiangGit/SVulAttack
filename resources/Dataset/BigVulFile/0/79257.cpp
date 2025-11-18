static int StreamTcpTest28(void)
{
    StreamTcpThread stt;
    StreamTcpUTInit(&stt.ra_ctx);

    uint32_t memuse = SC_ATOMIC_GET(st_memuse);

    StreamTcpIncrMemuse(500);
    FAIL_IF(SC_ATOMIC_GET(st_memuse) != (memuse+500));

    StreamTcpDecrMemuse(500);
    FAIL_IF(SC_ATOMIC_GET(st_memuse) != memuse);

    FAIL_IF(StreamTcpCheckMemcap(500) != 1);

    FAIL_IF(StreamTcpCheckMemcap((memuse + SC_ATOMIC_GET(stream_config.memcap))) != 0);

    StreamTcpUTDeinit(stt.ra_ctx);

    FAIL_IF(SC_ATOMIC_GET(st_memuse) != 0);
    PASS;
}
