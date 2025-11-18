void mp_buf_free(lua_State *L, mp_buf *buf) {
    mp_realloc(L, buf->b, buf->len + buf->free, 0); /* realloc to 0 = free */
    mp_realloc(L, buf, sizeof(*buf), 0);
}
