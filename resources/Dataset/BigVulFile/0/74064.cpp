dwarf_get_elf(Dwarf_Debug dbg, dwarf_elf_handle * elf,
    Dwarf_Error * error)
{
    struct Dwarf_Obj_Access_Interface_s * obj = 0;
    if (dbg == NULL) {
        _dwarf_error(NULL, error, DW_DLE_DBG_NULL);
        return (DW_DLV_ERROR);
    }

    obj = dbg->de_obj_file;
    if (obj) {
        dwarf_elf_object_access_internals_t *internals =
            (dwarf_elf_object_access_internals_t*)obj->object;
        if (internals->elf == NULL) {
            _dwarf_error(dbg, error, DW_DLE_FNO);
            return (DW_DLV_ERROR);
        }
        *elf = internals->elf;
        return DW_DLV_OK;

    }
    _dwarf_error(dbg, error, DW_DLE_FNO);
    return DW_DLV_ERROR;
}
