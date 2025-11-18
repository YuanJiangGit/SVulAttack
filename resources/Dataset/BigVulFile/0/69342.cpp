static int aes_xts_init_key(EVP_CIPHER_CTX *ctx, const unsigned char *key,
                            const unsigned char *iv, int enc)
{
    EVP_AES_XTS_CTX *xctx = EVP_C_DATA(EVP_AES_XTS_CTX,ctx);
    if (!iv && !key)
        return 1;

    if (key)
        do {
#ifdef AES_XTS_ASM
            xctx->stream = enc ? AES_xts_encrypt : AES_xts_decrypt;
#else
            xctx->stream = NULL;
#endif
            /* key_len is two AES keys */
#ifdef HWAES_CAPABLE
            if (HWAES_CAPABLE) {
                if (enc) {
                    HWAES_set_encrypt_key(key,
                                          EVP_CIPHER_CTX_key_length(ctx) * 4,
                                          &xctx->ks1.ks);
                    xctx->xts.block1 = (block128_f) HWAES_encrypt;
# ifdef HWAES_xts_encrypt
                    xctx->stream = HWAES_xts_encrypt;
# endif
                } else {
                    HWAES_set_decrypt_key(key,
                                          EVP_CIPHER_CTX_key_length(ctx) * 4,
                                          &xctx->ks1.ks);
                    xctx->xts.block1 = (block128_f) HWAES_decrypt;
# ifdef HWAES_xts_decrypt
                    xctx->stream = HWAES_xts_decrypt;
#endif
                }

                HWAES_set_encrypt_key(key + EVP_CIPHER_CTX_key_length(ctx) / 2,
                                      EVP_CIPHER_CTX_key_length(ctx) * 4,
                                      &xctx->ks2.ks);
                xctx->xts.block2 = (block128_f) HWAES_encrypt;

                xctx->xts.key1 = &xctx->ks1;
                break;
            } else
#endif
#ifdef BSAES_CAPABLE
            if (BSAES_CAPABLE)
                xctx->stream = enc ? bsaes_xts_encrypt : bsaes_xts_decrypt;
            else
#endif
#ifdef VPAES_CAPABLE
            if (VPAES_CAPABLE) {
                if (enc) {
                    vpaes_set_encrypt_key(key,
                                          EVP_CIPHER_CTX_key_length(ctx) * 4,
                                          &xctx->ks1.ks);
                    xctx->xts.block1 = (block128_f) vpaes_encrypt;
                } else {
                    vpaes_set_decrypt_key(key,
                                          EVP_CIPHER_CTX_key_length(ctx) * 4,
                                          &xctx->ks1.ks);
                    xctx->xts.block1 = (block128_f) vpaes_decrypt;
                }

                vpaes_set_encrypt_key(key + EVP_CIPHER_CTX_key_length(ctx) / 2,
                                      EVP_CIPHER_CTX_key_length(ctx) * 4,
                                      &xctx->ks2.ks);
                xctx->xts.block2 = (block128_f) vpaes_encrypt;

                xctx->xts.key1 = &xctx->ks1;
                break;
            } else
#endif
                (void)0;        /* terminate potentially open 'else' */

            if (enc) {
                AES_set_encrypt_key(key, EVP_CIPHER_CTX_key_length(ctx) * 4,
                                    &xctx->ks1.ks);
                xctx->xts.block1 = (block128_f) AES_encrypt;
            } else {
                AES_set_decrypt_key(key, EVP_CIPHER_CTX_key_length(ctx) * 4,
                                    &xctx->ks1.ks);
                xctx->xts.block1 = (block128_f) AES_decrypt;
            }

            AES_set_encrypt_key(key + EVP_CIPHER_CTX_key_length(ctx) / 2,
                                EVP_CIPHER_CTX_key_length(ctx) * 4,
                                &xctx->ks2.ks);
            xctx->xts.block2 = (block128_f) AES_encrypt;

            xctx->xts.key1 = &xctx->ks1;
        } while (0);

    if (iv) {
        xctx->xts.key2 = &xctx->ks2;
        memcpy(EVP_CIPHER_CTX_iv_noconst(ctx), iv, 16);
    }

    return 1;
}
