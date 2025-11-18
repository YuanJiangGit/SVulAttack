void ExtensionFunctionDispatcher::Dispatch(
    const ExtensionHostMsg_Request_Params& params,
    RenderViewHost* render_view_host) {
  ExtensionService* service = profile()->GetExtensionService();
  ExtensionProcessManager* process_manager =
      extensions::ExtensionSystem::Get(profile())->process_manager();
  extensions::ProcessMap* process_map = service->process_map();
  if (!service || !process_map)
    return;

  const Extension* extension = service->extensions()->GetByID(
      params.extension_id);
  if (!extension)
    extension = service->extensions()->GetHostedAppByURL(ExtensionURLInfo(
        WebSecurityOrigin::createFromString(params.source_origin),
        params.source_url));

  scoped_refptr<ExtensionFunction> function(
      CreateExtensionFunction(params, extension,
                              render_view_host->GetProcess()->GetID(),
                              *(service->process_map()),
                              extensions::ExtensionAPI::GetSharedInstance(),
                              profile(), render_view_host, render_view_host,
                              render_view_host->GetRoutingID()));
  scoped_ptr<ListValue> args(params.arguments.DeepCopy());

  if (!function) {
    LogFailure(extension,
               params.name,
               args.Pass(),
               kAccessDenied,
               profile());
    return;
  }

  UIThreadExtensionFunction* function_ui =
      function->AsUIThreadExtensionFunction();
  if (!function_ui) {
    NOTREACHED();
    return;
  }
  function_ui->set_dispatcher(AsWeakPtr());
  function_ui->set_profile(profile_);
  function->set_include_incognito(service->CanCrossIncognito(extension));

  if (!CheckPermissions(function, extension, params, render_view_host,
                        render_view_host->GetRoutingID())) {
    LogFailure(extension,
               params.name,
               args.Pass(),
               kAccessDenied,
               profile());
    return;
  }

  ExtensionsQuotaService* quota = service->quota_service();
  std::string violation_error = quota->Assess(extension->id(),
                                              function,
                                              &params.arguments,
                                              base::TimeTicks::Now());
  if (violation_error.empty()) {
    ExternalProtocolHandler::PermitLaunchUrl();
    LogSuccess(extension, params.name, args.Pass(), profile());
    function->Run();
  } else {
    LogFailure(extension,
               params.name,
               args.Pass(),
               kQuotaExceeded,
               profile());
    function->OnQuotaExceeded(violation_error);
  }


  if (!service->extensions()->GetByID(params.extension_id))
    return;

  process_manager->IncrementLazyKeepaliveCount(extension);
}
