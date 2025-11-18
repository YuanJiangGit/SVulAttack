static int search_chunk(struct mschmd_header *chm,
                        const unsigned char *chunk,
                        const char *filename,
                        const unsigned char **result,
                        const unsigned char **result_end)
{
    const unsigned char *start, *end, *p;
    unsigned int qr_size, num_entries, qr_entries, qr_density, name_len;
    unsigned int L, R, M, fname_len, entries_off, is_pmgl;
    int cmp;

    fname_len = strlen(filename);

    /* PMGL chunk or PMGI chunk? (note: read_chunk() has already
     * checked the rest of the characters in the chunk signature) */
    if (chunk[3] == 0x4C) {
        is_pmgl = 1;
        entries_off = pmgl_Entries;
    }
    else {
        is_pmgl = 0;
        entries_off = pmgi_Entries;
    }

    /*  Step 1: binary search first filename of each QR entry
     *  - target filename == entry
     *    found file
     *  - target filename < all entries
     *    file not found
     *  - target filename > all entries
     *    proceed to step 2 using final entry
     *  - target filename between two searched entries
     *    proceed to step 2
     */
    qr_size     = EndGetI32(&chunk[pmgl_QuickRefSize]);
    start       = &chunk[chm->chunk_size - 2];
    end         = &chunk[chm->chunk_size - qr_size];
    num_entries = EndGetI16(start);
    qr_density  = 1 + (1 << chm->density);
    qr_entries  = (num_entries + qr_density-1) / qr_density;

    if (num_entries == 0) {
        D(("chunk has no entries"))
        return -1;
    }

    if (qr_size > chm->chunk_size) {
        D(("quickref size > chunk size"))
        return -1;
    }

    *result_end = end;

    if (((int)qr_entries * 2) > (start - end)) {
        D(("WARNING; more quickrefs than quickref space"))
        qr_entries = 0; /* but we can live with it */
    }

    if (qr_entries > 0) {
        L = 0;
        R = qr_entries - 1;
        do {
            /* pick new midpoint */
            M = (L + R) >> 1;

            /* compare filename with entry QR points to */
            p = &chunk[entries_off + (M ? EndGetI16(start - (M << 1)) : 0)];
            READ_ENCINT(name_len);
            if (name_len > (unsigned int) (end - p)) goto chunk_end;
            cmp = compare(filename, (char *)p, fname_len, name_len);

            if (cmp == 0) break;
            else if (cmp < 0) { if (M) R = M - 1; else return 0; }
            else if (cmp > 0) L = M + 1;
        } while (L <= R);
        M = (L + R) >> 1;

        if (cmp == 0) {
            /* exact match! */
            p += name_len;
            *result = p;
            return 1;
        }

        /* otherwise, read the group of entries for QR entry M */
        p = &chunk[entries_off + (M ? EndGetI16(start - (M << 1)) : 0)];
        num_entries -= (M * qr_density);
        if (num_entries > qr_density) num_entries = qr_density;
    }
    else {
        p = &chunk[entries_off];
    }

    /* Step 2: linear search through the set of entries reached in step 1.
     * - filename == any entry
     *   found entry
     * - filename < all entries (PMGI) or any entry (PMGL)
     *   entry not found, stop now
     * - filename > all entries
     *   entry not found (PMGL) / maybe found (PMGI)
     * - 
     */
    *result = NULL;
    while (num_entries-- > 0) {
        READ_ENCINT(name_len);
        if (name_len > (unsigned int) (end - p)) goto chunk_end;
        cmp = compare(filename, (char *)p, fname_len, name_len);
        p += name_len;

        if (cmp == 0) {
            /* entry found */
            *result = p;
            return 1;
        }

        if (cmp < 0) {
            /* entry not found (PMGL) / maybe found (PMGI) */
            break;
        }

        /* read and ignore the rest of this entry */
        if (is_pmgl) {
            READ_ENCINT(R); /* skip section */
            READ_ENCINT(R); /* skip offset */
            READ_ENCINT(R); /* skip length */
        }
        else {
            *result = p; /* store potential final result */
            READ_ENCINT(R); /* skip chunk number */
        }
    }

     /* PMGL? not found. PMGI? maybe found */
     return (is_pmgl) ? 0 : (*result ? 1 : 0);

 chunk_end:
    D(("reached end of chunk data while searching"))
    return -1;
}
