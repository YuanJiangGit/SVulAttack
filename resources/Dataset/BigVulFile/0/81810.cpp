robj *streamTypeLookupWriteOrCreate(client *c, robj *key) {
    robj *o = lookupKeyWrite(c->db,key);
    if (o == NULL) {
        o = createStreamObject();
        dbAdd(c->db,key,o);
    } else {
        if (o->type != OBJ_STREAM) {
            addReply(c,shared.wrongtypeerr);
            return NULL;
        }
    }
    return o;
}
