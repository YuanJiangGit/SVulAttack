void address_space_stb(AddressSpace *as, hwaddr addr, uint32_t val,
                       MemTxAttrs attrs, MemTxResult *result)
{
    uint8_t v = val;
    MemTxResult r;

    r = address_space_rw(as, addr, attrs, &v, 1, 1);
    if (result) {
        *result = r;
    }
}
