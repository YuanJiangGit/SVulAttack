static int lua_ap_loaded_modules(lua_State *L)
{
    int i;
    lua_newtable(L);
    for (i = 0; ap_loaded_modules[i] && ap_loaded_modules[i]->name; i++) {
        lua_pushinteger(L, i + 1);
        lua_pushstring(L, ap_loaded_modules[i]->name);
        lua_settable(L, -3);
    }
    return 1;
}
