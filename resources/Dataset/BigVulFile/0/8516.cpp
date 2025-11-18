static void hash_mpint(const struct ssh_hash *h, void *s, Bignum b)
{
    unsigned char *p;
    int len;
    p = ssh2_mpint_fmt(b, &len);
    hash_string(h, s, p, len);
    sfree(p);
}
