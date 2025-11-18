void TestingAutomationProvider::GetEnterprisePolicyInfo(
    DictionaryValue* args, IPC::Message* reply_message) {
  AutomationJSONReply reply(this, reply_message);
  scoped_ptr<DictionaryValue> return_value(new DictionaryValue);

  policy::BrowserPolicyConnector* connector =
      g_browser_process->browser_policy_connector();
  if (!connector) {
    reply.SendError("Unable to access BrowserPolicyConnector");
    return;
  }
  policy::CloudPolicySubsystem* user_cloud_policy =
      connector->user_cloud_policy_subsystem();
  policy::CloudPolicySubsystem* device_cloud_policy =
      connector->device_cloud_policy_subsystem();
  const policy::CloudPolicyDataStore* user_data_store =
      connector->GetUserCloudPolicyDataStore();
  const policy::CloudPolicyDataStore* device_data_store =
      connector->GetDeviceCloudPolicyDataStore();
  if (!user_cloud_policy || !device_cloud_policy) {
    reply.SendError("Unable to access a CloudPolicySubsystem");
    return;
  }
  if (!user_data_store || !device_data_store) {
    reply.SendError("Unable to access a CloudPolicyDataStore");
    return;
  }

  return_value->SetString("enterprise_domain",
                          connector->GetEnterpriseDomain());
  return_value->SetString("user_cloud_policy",
      EnterpriseStatusToString(user_cloud_policy->state()));
  return_value->SetString("device_cloud_policy",
      EnterpriseStatusToString(device_cloud_policy->state()));
  return_value->SetString("device_id", device_data_store->device_id());
  return_value->SetString("device_token", device_data_store->device_token());
  return_value->SetString("gaia_token", device_data_store->gaia_token());
  return_value->SetString("machine_id", device_data_store->machine_id());
  return_value->SetString("machine_model", device_data_store->machine_model());
  return_value->SetString("user_name", device_data_store->user_name());
  return_value->SetBoolean("device_token_cache_loaded",
                           device_data_store->token_cache_loaded());
  return_value->SetBoolean("user_token_cache_loaded",
                           user_data_store->token_cache_loaded());
  return_value->Set("device_mandatory_policies",
                    CreateDictionaryWithPolicies(device_cloud_policy,
                        policy::CloudPolicyCacheBase::POLICY_LEVEL_MANDATORY));
  return_value->Set("user_mandatory_policies",
                    CreateDictionaryWithPolicies(user_cloud_policy,
                        policy::CloudPolicyCacheBase::POLICY_LEVEL_MANDATORY));
  return_value->Set("device_recommended_policies",
      CreateDictionaryWithPolicies(device_cloud_policy,
          policy::CloudPolicyCacheBase::POLICY_LEVEL_RECOMMENDED));
  return_value->Set("user_recommended_policies",
      CreateDictionaryWithPolicies(user_cloud_policy,
          policy::CloudPolicyCacheBase::POLICY_LEVEL_RECOMMENDED));
  reply.SendSuccess(return_value.get());
}
