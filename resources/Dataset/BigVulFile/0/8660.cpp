size_t mptsas_config_phy_0(MPTSASState *s, uint8_t **data, int address)
{
    int phy_handle = -1;
    int dev_handle = -1;
    int i = mptsas_phy_addr_get(s, address);
    SCSIDevice *dev;

    if (i < 0) {
        trace_mptsas_config_sas_phy(s, address, i, phy_handle, dev_handle, 0);
        return i;
    }

    dev = mptsas_phy_get_device(s, i, &phy_handle, &dev_handle);
    trace_mptsas_config_sas_phy(s, address, i, phy_handle, dev_handle, 0);

    return MPTSAS_CONFIG_PACK_EXT(0, MPI_CONFIG_EXTPAGETYPE_SAS_PHY, 0x01,
                                  "w*wqwb*blbb*b*b*l",
                                  dev_handle, s->sas_addr, dev_handle, i,
                                  (dev
                                   ? MPI_SAS_DEVICE_INFO_END_DEVICE /* | MPI_SAS_DEVICE_INFO_SSP_TARGET?? */
                                   : MPI_SAS_DEVICE_INFO_NO_DEVICE),
                                  (MPI_SAS_IOUNIT0_RATE_3_0 << 4) | MPI_SAS_IOUNIT0_RATE_1_5,
                                  (MPI_SAS_IOUNIT0_RATE_3_0 << 4) | MPI_SAS_IOUNIT0_RATE_1_5);
}
