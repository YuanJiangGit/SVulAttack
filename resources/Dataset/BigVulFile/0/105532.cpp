bool SendAcceptPromptAppModalDialogJSONRequest(
    AutomationMessageSender* sender,
    const std::string& prompt_text,
    std::string* error_msg) {
  DictionaryValue dict;
  dict.SetString("command", "AcceptOrDismissAppModalDialog");
  dict.SetBoolean("accept", true);
  dict.SetString("prompt_text", prompt_text);
  DictionaryValue reply_dict;
  return SendAutomationJSONRequest(sender, dict, &reply_dict, error_msg);
}
