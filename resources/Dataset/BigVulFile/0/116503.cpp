bool SendCommandDebuggerFunction::RunImpl() {

  if (!InitClientHost())
    return false;

  std::string method;
  EXTENSION_FUNCTION_VALIDATE(args_->GetString(1, &method));

  Value *params;
  if (!args_->Get(2, &params))
    params = NULL;

  client_host_->SendMessageToBackend(this, method, params);
  return true;
}
