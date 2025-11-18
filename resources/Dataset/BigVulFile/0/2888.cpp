get_global_op_array(const gs_memory_t *mem)
{
    gs_main_instance *minst = get_minst_from_memory(mem);
    return &minst->i_ctx_p->op_array_table_global;
}
