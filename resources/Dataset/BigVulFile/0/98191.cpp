void TranslateManager::ReportLanguageDetectionError(TabContents* tab_contents) {
  UMA_HISTOGRAM_COUNTS("Translate.ReportLanguageDetectionError", 1);
  GURL page_url = tab_contents->controller().GetActiveEntry()->url();
  std::string report_error_url(kReportLanguageDetectionErrorURL);
  report_error_url += "?client=cr&action=langidc&u=";
  report_error_url += EscapeUrlEncodedData(page_url.spec());
  report_error_url += "&sl=";
  report_error_url += tab_contents->language_state().original_language();
  report_error_url += "&hl=";
  report_error_url +=
      GetLanguageCode(g_browser_process->GetApplicationLocale());
  Browser* browser = BrowserList::GetLastActive();
  if (!browser) {
    NOTREACHED();
    return;
  }
  browser->AddTabWithURL(GURL(report_error_url), GURL(),
                         PageTransition::AUTO_BOOKMARK, -1,
                         TabStripModel::ADD_SELECTED, NULL, std::string(),
                         NULL);
}
