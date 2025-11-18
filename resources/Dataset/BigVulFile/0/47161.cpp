static void __exit camellia_fini(void)
{
	crypto_unregister_alg(&camellia_alg);
}
