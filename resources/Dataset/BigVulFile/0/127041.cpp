PassRefPtr<DateTimeChooser> ChromeClientImpl::openDateTimeChooser(DateTimeChooserClient* pickerClient, const DateTimeChooserParameters& parameters)
{
#if ENABLE(INPUT_MULTIPLE_FIELDS_UI)
    return DateTimeChooserImpl::create(this, pickerClient, parameters);
#else
    return ExternalDateTimeChooser::create(this, m_webView->client(), pickerClient, parameters);
#endif
}
