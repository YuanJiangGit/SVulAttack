uiserver_set_protocol (void *engine, gpgme_protocol_t protocol)
{
  engine_uiserver_t uiserver = engine;

  if (protocol != GPGME_PROTOCOL_OpenPGP
      && protocol != GPGME_PROTOCOL_CMS
      && protocol != GPGME_PROTOCOL_DEFAULT)
    return gpg_error (GPG_ERR_INV_VALUE);

  uiserver->protocol = protocol;
  return 0;
}
