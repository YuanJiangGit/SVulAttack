void ChromeContentBrowserClient::ClearCookies(RenderViewHost* rvh) {
  Profile* profile = Profile::FromBrowserContext(
      rvh->GetSiteInstance()->GetProcess()->GetBrowserContext());
  BrowsingDataRemover* remover =
      BrowsingDataRemover::CreateForUnboundedRange(profile);
  int remove_mask = BrowsingDataRemover::REMOVE_SITE_DATA;
  remover->Remove(remove_mask, BrowsingDataHelper::UNPROTECTED_WEB);
}
