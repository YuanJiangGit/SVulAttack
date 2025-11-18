static int __init seqiv_module_init(void)
{
	return crypto_register_template(&seqiv_tmpl);
}
