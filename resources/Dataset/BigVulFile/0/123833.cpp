  bool WaitForExtensionViewsToLoad() {

    content::NotificationRegistrar registrar;
    registrar.Add(this, chrome::NOTIFICATION_EXTENSION_HOST_DID_STOP_LOADING,
                  content::NotificationService::AllSources());
    base::CancelableClosure timeout(
        base::Bind(&TimeoutCallback, "Extension host load timed out."));
    MessageLoop::current()->PostDelayedTask(
        FROM_HERE, timeout.callback(), base::TimeDelta::FromSeconds(4));

    ExtensionProcessManager* manager =
        extensions::ExtensionSystem::Get(browser()->profile())->
            process_manager();
    ExtensionProcessManager::ViewSet all_views = manager->GetAllViews();
    for (ExtensionProcessManager::ViewSet::const_iterator iter =
             all_views.begin();
         iter != all_views.end();) {
      if (!(*iter)->IsLoading())
        ++iter;
      else
        content::RunMessageLoop();
    }

    timeout.Cancel();
    return true;
  }
