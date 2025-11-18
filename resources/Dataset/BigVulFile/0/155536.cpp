AuthenticatorGenericErrorSheetModel::ForMissingUserVerificationSupport(
    AuthenticatorRequestDialogModel* dialog_model) {
  return base::WrapUnique(new AuthenticatorGenericErrorSheetModel(
      dialog_model,
      l10n_util::GetStringUTF16(IDS_WEBAUTHN_ERROR_MISSING_CAPABILITY_TITLE),
      l10n_util::GetStringFUTF16(IDS_WEBAUTHN_MISSING_USER_VERIFICATION_DESC,
                                 GetRelyingPartyIdString(dialog_model))));
}
