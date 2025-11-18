void OneClickSigninSyncStarter::SigninDialogDelegate::OnContinueSignin() {
  sync_starter_->LoadPolicyWithCachedClient();
}
