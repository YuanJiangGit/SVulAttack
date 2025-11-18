int parse_bytes(const char *t, off_t *bytes) {
        static const struct {
                const char *suffix;
                off_t factor;
        } table[] = {
                { "B", 1 },
                { "K", 1024ULL },
                { "M", 1024ULL*1024ULL },
                { "G", 1024ULL*1024ULL*1024ULL },
                { "T", 1024ULL*1024ULL*1024ULL*1024ULL },
                { "P", 1024ULL*1024ULL*1024ULL*1024ULL*1024ULL },
                { "E", 1024ULL*1024ULL*1024ULL*1024ULL*1024ULL*1024ULL },
                { "", 1 },
        };

        const char *p;
        off_t r = 0;

        assert(t);
        assert(bytes);

        p = t;
        do {
                long long l;
                char *e;
                unsigned i;

                errno = 0;
                l = strtoll(p, &e, 10);

                if (errno != 0)
                        return -errno;

                if (l < 0)
                        return -ERANGE;

                if (e == p)
                        return -EINVAL;

                e += strspn(e, WHITESPACE);

                for (i = 0; i < ELEMENTSOF(table); i++)
                        if (startswith(e, table[i].suffix)) {
                                r += (off_t) l * table[i].factor;
                                p = e + strlen(table[i].suffix);
                                break;
                        }

                if (i >= ELEMENTSOF(table))
                        return -EINVAL;

        } while (*p != 0);

        *bytes = r;

        return 0;
}
