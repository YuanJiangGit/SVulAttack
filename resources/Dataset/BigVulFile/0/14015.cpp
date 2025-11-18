gs_lib_ctx_t *gs_lib_ctx_get_interp_instance(const gs_memory_t *mem)
{
    if (mem == NULL)
        return NULL;
    return mem->gs_lib_ctx;
}
