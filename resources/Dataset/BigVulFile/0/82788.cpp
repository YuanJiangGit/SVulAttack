static int esil_avr_hook_reg_write(RAnalEsil *esil, const char *name, ut64 *val) {
	CPU_MODEL *cpu;

	if (!esil || !esil->anal) {
		return 0;
	}

	cpu = get_cpu_model (esil->anal->cpu);

	if (!strcmp (name, "pc")) {
		*val &= CPU_PC_MASK (cpu);
	} else if (!strcmp (name, "pcl")) {
		if (cpu->pc < 8) {
			*val &= MASK (8);
		}
	} else if (!strcmp (name, "pch")) {
		*val = cpu->pc > 8
			? *val & MASK (cpu->pc - 8)
			: 0;
	}

	return 0;
}
