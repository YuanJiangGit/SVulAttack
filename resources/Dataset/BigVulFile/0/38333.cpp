static inline bool is_imm8(int value)
{
	return value <= 127 && value >= -128;
}
