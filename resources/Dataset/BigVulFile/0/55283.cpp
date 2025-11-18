static int atl2_check_eeprom_exist(struct atl2_hw *hw)
{
	u32 value;

	value = ATL2_READ_REG(hw, REG_SPI_FLASH_CTRL);
	if (value & SPI_FLASH_CTRL_EN_VPD) {
		value &= ~SPI_FLASH_CTRL_EN_VPD;
		ATL2_WRITE_REG(hw, REG_SPI_FLASH_CTRL, value);
	}
	value = ATL2_READ_REGW(hw, REG_PCIE_CAP_LIST);
	return ((value & 0xFF00) == 0x6C00) ? 0 : 1;
}
