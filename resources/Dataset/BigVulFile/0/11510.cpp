int close_pipe(int p[]) {
        int a = 0, b = 0;

        assert(p);

        if (p[0] >= 0) {
                a = close_nointr(p[0]);
                p[0] = -1;
        }

        if (p[1] >= 0) {
                b = close_nointr(p[1]);
                p[1] = -1;
        }

        return a < 0 ? a : b;
}
