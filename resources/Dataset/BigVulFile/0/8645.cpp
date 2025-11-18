size_t mptsas_config_ioc_4(MPTSASState *s, uint8_t **data, int address)
{
    return MPTSAS_CONFIG_PACK(4, MPI_CONFIG_PAGETYPE_IOC, 0x00,
                              "*b*b*w");
}
