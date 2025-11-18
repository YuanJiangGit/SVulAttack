base::string16 AuthenticatorTouchIdIncognitoBumpSheetModel::GetStepDescription()
    const {
#if defined(OS_MACOSX)
  return l10n_util::GetStringUTF16(
      IDS_WEBAUTHN_TOUCH_ID_INCOGNITO_BUMP_DESCRIPTION);
#else
  return base::string16();
#endif  // defined(OS_MACOSX)
}
