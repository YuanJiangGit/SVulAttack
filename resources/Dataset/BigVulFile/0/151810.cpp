  std::unique_ptr<VisibleSecurityState> BaseVisibleSecurityState() {
    auto visible_security_state = std::make_unique<VisibleSecurityState>();
    visible_security_state->connection_info_initialized = true;
    visible_security_state->url = kHttpsUrl;
    visible_security_state->certificate =
        net::ImportCertFromFile(net::GetTestCertsDirectory(), "sha1_2016.pem");
    visible_security_state->cert_status =
        net::CERT_STATUS_SHA1_SIGNATURE_PRESENT;
    return visible_security_state;
  }
