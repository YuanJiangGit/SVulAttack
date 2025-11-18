void v9fs_reclaim_fd(V9fsPDU *pdu)
{
    int reclaim_count = 0;
    V9fsState *s = pdu->s;
    V9fsFidState *f, *reclaim_list = NULL;

    for (f = s->fid_list; f; f = f->next) {
        /*
         * Unlink fids cannot be reclaimed. Check
         * for them and skip them. Also skip fids
         * currently being operated on.
         */
        if (f->ref || f->flags & FID_NON_RECLAIMABLE) {
            continue;
        }
        /*
         * if it is a recently referenced fid
         * we leave the fid untouched and clear the
         * reference bit. We come back to it later
         * in the next iteration. (a simple LRU without
         * moving list elements around)
         */
        if (f->flags & FID_REFERENCED) {
            f->flags &= ~FID_REFERENCED;
            continue;
        }
        /*
         * Add fids to reclaim list.
         */
        if (f->fid_type == P9_FID_FILE) {
            if (f->fs.fd != -1) {
                /*
                 * Up the reference count so that
                 * a clunk request won't free this fid
                 */
                f->ref++;
                f->rclm_lst = reclaim_list;
                reclaim_list = f;
                f->fs_reclaim.fd = f->fs.fd;
                f->fs.fd = -1;
                reclaim_count++;
            }
        } else if (f->fid_type == P9_FID_DIR) {
            if (f->fs.dir.stream != NULL) {
                /*
                 * Up the reference count so that
                 * a clunk request won't free this fid
                 */
                f->ref++;
                f->rclm_lst = reclaim_list;
                reclaim_list = f;
                f->fs_reclaim.dir.stream = f->fs.dir.stream;
                f->fs.dir.stream = NULL;
                reclaim_count++;
            }
        }
        if (reclaim_count >= open_fd_rc) {
            break;
        }
    }
    /*
     * Now close the fid in reclaim list. Free them if they
     * are already clunked.
     */
    while (reclaim_list) {
        f = reclaim_list;
        reclaim_list = f->rclm_lst;
        if (f->fid_type == P9_FID_FILE) {
            v9fs_co_close(pdu, &f->fs_reclaim);
        } else if (f->fid_type == P9_FID_DIR) {
            v9fs_co_closedir(pdu, &f->fs_reclaim);
        }
        f->rclm_lst = NULL;
        /*
         * Now drop the fid reference, free it
         * if clunked.
         */
        put_fid(pdu, f);
    }
}
