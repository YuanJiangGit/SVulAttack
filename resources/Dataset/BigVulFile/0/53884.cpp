void __init acpi_osi_setup(char *str)
{
	struct osi_setup_entry *osi;
	bool enable = true;
	int i;

	if (!acpi_gbl_create_osi_method)
		return;

	if (str == NULL || *str == '\0') {
		printk(KERN_INFO PREFIX "_OSI method disabled\n");
		acpi_gbl_create_osi_method = FALSE;
		return;
	}

	if (*str == '!') {
		str++;
		if (*str == '\0') {
			osi_linux.default_disabling = 1;
			return;
		} else if (*str == '*') {
			acpi_update_interfaces(ACPI_DISABLE_ALL_STRINGS);
			for (i = 0; i < OSI_STRING_ENTRIES_MAX; i++) {
				osi = &osi_setup_entries[i];
				osi->enable = false;
			}
			return;
		}
		enable = false;
	}

	for (i = 0; i < OSI_STRING_ENTRIES_MAX; i++) {
		osi = &osi_setup_entries[i];
		if (!strcmp(osi->string, str)) {
			osi->enable = enable;
			break;
		} else if (osi->string[0] == '\0') {
			osi->enable = enable;
			strncpy(osi->string, str, OSI_STRING_LENGTH_MAX);
			break;
		}
	}
}
