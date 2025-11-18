static int opinc(RAsm *a, ut8 *data, const Opcode *op) {
	if (op->operands[1].type) {
		eprintf ("Error: Invalid operands\n");
		return -1;
	}
	int l = 0;
	int size = op->operands[0].type & ALL_SIZE;
	if (op->operands[0].explicit_size) {
		size = op->operands[0].dest_size;
	}

	if (size & OT_WORD) {
		data[l++] = 0x66;
	}

	int rex = 1 << 6;
	bool use_rex = false;
	if (size & OT_QWORD) {			//W field
		use_rex = true;
		rex |= 1 << 3;
	}
	if (op->operands[0].extended) {		//B field
		use_rex = true;
		rex |= 1;
	}

	int opcode;
	if (size & OT_BYTE) {
		opcode = 0xfe;
	} else {
		opcode = 0xff;
	}

	if (!(op->operands[0].type & OT_MEMORY)) {
		if (use_rex) {
			data[l++] = rex;
		}
		if (a->bits > 32 || size & OT_BYTE) {
			data[l++] = opcode;
		}
		if (a->bits == 32 && size & (OT_DWORD | OT_WORD)) {
			data[l++] = 0x40 | op->operands[0].reg;
		} else {
			data[l++] = 0xc0 | op->operands[0].reg;
		}
		return l;
	}

	bool rip_rel = op->operands[0].regs[0] == X86R_RIP;
	int offset = op->operands[0].offset * op->operands[0].offset_sign;
	int modrm = 0;
	int mod;
	int reg = 0;
	int rm;
	bool use_sib = false;
	int sib = 0;
	if (offset == 0) {
		mod = 0;
	} else if (offset < 128 && offset > -129) {
		mod = 1;
	} else {
		mod = 2;
	}

	if (op->operands[0].regs[0] & OT_WORD) {
		if (op->operands[0].regs[0] == X86R_BX && op->operands[0].regs[1] == X86R_SI) {
			rm = B0000;
		} else if (op->operands[0].regs[0] == X86R_BX && op->operands[0].regs[1] == X86R_DI) {
			rm = B0001;
		} else if (op->operands[0].regs[0] == X86R_BP && op->operands[0].regs[1] == X86R_SI) {
			rm = B0010;
		} else if (op->operands[0].regs[0] == X86R_BP && op->operands[0].regs[1] == X86R_DI) {
			rm = B0011;
		} else if (op->operands[0].regs[0] == X86R_SI && op->operands[0].regs[1] == -1) {
			rm = B0100;
		} else if (op->operands[0].regs[0] == X86R_DI && op->operands[0].regs[1] == -1) {
			rm = B0101;
		} else if (op->operands[0].regs[0] == X86R_BX && op->operands[0].regs[1] == -1) {
			rm = B0111;
		} else {
			return -1;
		}
		modrm = (mod << 6) | (reg << 3) | rm;
	} else {
		if (op->operands[0].extended) {
			rm = op->operands[0].reg;
		} else {
			rm = op->operands[0].regs[0];
		}
		if (rm == 5 && mod == 0) {
			mod = 1;
		}

		int index = op->operands[0].regs[1];
		int scale = getsib(op->operands[0].scale[1]);
		if (index != -1) {
			use_sib = true;
			sib = (scale << 6) | (index << 3) | rm;
		} else if (rm == 4) {
			use_sib = true;
			sib = 0x24;
		}
		if (use_sib) {
			rm = B0100;
		}
		if (rip_rel) {
			modrm = (B0000 << 6) | (reg << 3) | B0101;
			sib = (scale << 6) | (B0100 << 3) | B0101;
		} else {
			modrm = (mod << 6) | (reg << 3) | rm;
		}
	}

	if (use_rex) {
		data[l++] = rex;
	}
	data[l++] = opcode;
	data[l++] = modrm;
	if (use_sib) {
		data[l++] = sib;
	}
	if (mod == 1) {
		data[l++] = offset;
	} else if (op->operands[0].regs[0] & OT_WORD && mod == 2) {
		data[l++] = offset;
		data[l++] = offset >> 8;
	} else if (mod == 2 || rip_rel) {
		data[l++] = offset;
		data[l++] = offset >> 8;
		data[l++] = offset >> 16;
		data[l++] = offset >> 24;
	}

	return l;
}
