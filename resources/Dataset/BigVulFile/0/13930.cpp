gs_get_colorname_string(const gs_memory_t *mem, gs_separation_name colorname_index,
                        unsigned char **ppstr, unsigned int *pname_size)
{
    ref nref;

    name_index_ref(mem, colorname_index, &nref);
    name_string_ref(mem, &nref, &nref);
    return obj_string_data(mem, &nref, (const unsigned char**) ppstr, pname_size);
}
