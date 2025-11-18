static int coroutine_fn v9fs_mark_fids_unreclaim(V9fsPDU *pdu, V9fsPath *path)
{
    int err;
    V9fsState *s = pdu->s;
    V9fsFidState *fidp, head_fid;

    head_fid.next = s->fid_list;
    for (fidp = s->fid_list; fidp; fidp = fidp->next) {
        if (fidp->path.size != path->size) {
            continue;
        }
        if (!memcmp(fidp->path.data, path->data, path->size)) {
            /* Mark the fid non reclaimable. */
            fidp->flags |= FID_NON_RECLAIMABLE;

            /* reopen the file/dir if already closed */
            err = v9fs_reopen_fid(pdu, fidp);
            if (err < 0) {
                return err;
            }
            /*
             * Go back to head of fid list because
             * the list could have got updated when
             * switched to the worker thread
             */
            if (err == 0) {
                fidp = &head_fid;
            }
        }
    }
    return 0;
}
