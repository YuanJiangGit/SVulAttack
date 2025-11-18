assemble_algorithms(ServerOptions *o)
{
	if (kex_assemble_names(KEX_SERVER_ENCRYPT, &o->ciphers) != 0 ||
	    kex_assemble_names(KEX_SERVER_MAC, &o->macs) != 0 ||
	    kex_assemble_names(KEX_SERVER_KEX, &o->kex_algorithms) != 0 ||
	    kex_assemble_names(KEX_DEFAULT_PK_ALG,
	    &o->hostkeyalgorithms) != 0 ||
	    kex_assemble_names(KEX_DEFAULT_PK_ALG,
	    &o->hostbased_key_types) != 0 ||
	    kex_assemble_names(KEX_DEFAULT_PK_ALG, &o->pubkey_key_types) != 0)
		fatal("kex_assemble_names failed");
}
