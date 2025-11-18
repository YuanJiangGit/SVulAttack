void ResetScreenHandler::ShowWithParams() {
  int dialog_type;
  if (reboot_was_requested_) {
    dialog_type = rollback_available_ ?
        reset::DIALOG_SHORTCUT_CONFIRMING_POWERWASH_AND_ROLLBACK :
        reset::DIALOG_SHORTCUT_CONFIRMING_POWERWASH_ONLY;
  } else {
    dialog_type = rollback_available_ ?
      reset::DIALOG_SHORTCUT_OFFERING_ROLLBACK_AVAILABLE :
      reset::DIALOG_SHORTCUT_OFFERING_ROLLBACK_UNAVAILABLE;
  }
  UMA_HISTOGRAM_ENUMERATION("Reset.ChromeOS.PowerwashDialogShown",
                            dialog_type,
                            reset::DIALOG_VIEW_TYPE_SIZE);

  base::DictionaryValue reset_screen_params;
  reset_screen_params.SetBoolean("showRestartMsg", restart_required_);
  reset_screen_params.SetBoolean(
      "showRollbackOption", rollback_available_ && !reboot_was_requested_);
  reset_screen_params.SetBoolean(
      "simpleConfirm", reboot_was_requested_ && !rollback_available_);
  reset_screen_params.SetBoolean(
      "rollbackConfirm", reboot_was_requested_ && rollback_available_);

  PrefService* prefs = g_browser_process->local_state();
  prefs->SetBoolean(prefs::kFactoryResetRequested, false);
  prefs->SetBoolean(prefs::kRollbackRequested, false);
  prefs->CommitPendingWrite();
  ShowScreen(kResetScreen, &reset_screen_params);
}
