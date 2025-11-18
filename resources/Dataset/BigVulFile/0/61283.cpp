EXPORTED int mboxlist_lookup(const char *name, mbentry_t **entryptr,
                             struct txn **tid)
{
    mbentry_t *entry = NULL;
    int r;

    r = mboxlist_mylookup(name, &entry, tid, 0);

    if (r) return r;

    /* Ignore "reserved" entries, like they aren't there */
    if (entry->mbtype & MBTYPE_RESERVE) {
        mboxlist_entry_free(&entry);
        return IMAP_MAILBOX_RESERVED;
    }

    /* Ignore "deleted" entries, like they aren't there */
    if (entry->mbtype & MBTYPE_DELETED) {
        mboxlist_entry_free(&entry);
        return IMAP_MAILBOX_NONEXISTENT;
    }

    if (entryptr) *entryptr = entry;
    else mboxlist_entry_free(&entry);

    return 0;
}
