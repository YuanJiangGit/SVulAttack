bool OSExchangeData::GetHtml(base::string16* html, GURL* base_url) const {
  return provider_->GetHtml(html, base_url);
}
