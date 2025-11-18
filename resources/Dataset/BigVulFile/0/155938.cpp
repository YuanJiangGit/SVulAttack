void PeopleHandler::HandleStartSyncingWithEmail(const base::ListValue* args) {
  DCHECK(AccountConsistencyModeManager::IsDiceEnabledForProfile(profile_));
  const base::Value* email;
  const base::Value* is_default_promo_account;
  CHECK(args->Get(0, &email));
  CHECK(args->Get(1, &is_default_promo_account));

  Browser* browser =
      chrome::FindBrowserWithWebContents(web_ui()->GetWebContents());

  AccountTrackerService* account_tracker =
      AccountTrackerServiceFactory::GetForProfile(profile_);
  AccountInfo account =
      account_tracker->FindAccountInfoByEmail(email->GetString());
  signin_ui_util::EnableSyncFromPromo(
      browser, account, signin_metrics::AccessPoint::ACCESS_POINT_SETTINGS,
      is_default_promo_account->GetBool());
}
