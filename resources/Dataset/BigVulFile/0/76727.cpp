checkAttr_safe(const InString *input, int pos,
		const TranslationTableCharacterAttributes a, int m,
		const TranslationTableHeader *table) {
	return ((pos < input->length) ? checkAttr(input->chars[pos], a, m, table) : 0);
}
