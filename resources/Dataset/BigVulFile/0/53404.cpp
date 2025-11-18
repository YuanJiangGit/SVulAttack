iperf_new_stream(struct iperf_test *test, int s)
{
    int i;
    struct iperf_stream *sp;
    char template[] = "/tmp/iperf3.XXXXXX";

    h_errno = 0;

    sp = (struct iperf_stream *) malloc(sizeof(struct iperf_stream));
    if (!sp) {
        i_errno = IECREATESTREAM;
        return NULL;
    }

    memset(sp, 0, sizeof(struct iperf_stream));

    sp->test = test;
    sp->settings = test->settings;
    sp->result = (struct iperf_stream_result *) malloc(sizeof(struct iperf_stream_result));
    if (!sp->result) {
        free(sp);
        i_errno = IECREATESTREAM;
        return NULL;
    }

    memset(sp->result, 0, sizeof(struct iperf_stream_result));
    TAILQ_INIT(&sp->result->interval_results);
    
    /* Create and randomize the buffer */
    sp->buffer_fd = mkstemp(template);
    if (sp->buffer_fd == -1) {
        i_errno = IECREATESTREAM;
        free(sp->result);
        free(sp);
        return NULL;
    }
    if (unlink(template) < 0) {
        i_errno = IECREATESTREAM;
        free(sp->result);
        free(sp);
        return NULL;
    }
    if (ftruncate(sp->buffer_fd, test->settings->blksize) < 0) {
        i_errno = IECREATESTREAM;
        free(sp->result);
        free(sp);
        return NULL;
    }
    sp->buffer = (char *) mmap(NULL, test->settings->blksize, PROT_READ|PROT_WRITE, MAP_PRIVATE, sp->buffer_fd, 0);
    if (sp->buffer == MAP_FAILED) {
        i_errno = IECREATESTREAM;
        free(sp->result);
        free(sp);
        return NULL;
    }
    srandom(time(NULL));
    for (i = 0; i < test->settings->blksize; ++i)
        sp->buffer[i] = random();

    /* Set socket */
    sp->socket = s;

    sp->snd = test->protocol->send;
    sp->rcv = test->protocol->recv;

    if (test->diskfile_name != (char*) 0) {
	sp->diskfile_fd = open(test->diskfile_name, test->sender ? O_RDONLY : (O_WRONLY|O_CREAT|O_TRUNC), S_IRUSR|S_IWUSR);
	if (sp->diskfile_fd == -1) {
	    i_errno = IEFILE;
            munmap(sp->buffer, sp->test->settings->blksize);
            free(sp->result);
            free(sp);
	    return NULL;
	}
        sp->snd2 = sp->snd;
	sp->snd = diskfile_send;
	sp->rcv2 = sp->rcv;
	sp->rcv = diskfile_recv;
    } else
        sp->diskfile_fd = -1;

    /* Initialize stream */
    if (iperf_init_stream(sp, test) < 0) {
        close(sp->buffer_fd);
        munmap(sp->buffer, sp->test->settings->blksize);
        free(sp->result);
        free(sp);
        return NULL;
    }
    iperf_add_stream(test, sp);

    return sp;
}
