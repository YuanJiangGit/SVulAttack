int SSL_in_init(SSL *s)
{
    return s->statem.in_init;
}
