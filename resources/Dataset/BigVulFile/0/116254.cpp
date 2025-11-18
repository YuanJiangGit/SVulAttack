bool QQuickWebViewPrivate::handleCertificateVerificationRequest(const QString& hostname)
{
    Q_Q(QQuickWebView);

    if (m_allowAnyHTTPSCertificateForLocalHost
        && (hostname == QStringLiteral("127.0.0.1") || hostname == QStringLiteral("localhost")))
        return true;

    QtDialogRunner dialogRunner(q);
    if (!dialogRunner.initForCertificateVerification(hostname))
        return false;

    dialogRunner.run();

    return dialogRunner.wasAccepted();
}
