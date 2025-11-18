static jboolean android_net_wifi_setLazyRoam(
 JNIEnv *env, jclass cls, jint iface, jint id, jboolean enabled, jobject roam_param) {

 JNIHelper helper(env);
    wifi_error status = WIFI_SUCCESS;
    wifi_roam_params params;
    memset(&params, 0, sizeof(params));

    wifi_interface_handle handle = getIfaceHandle(helper, cls, iface);
    ALOGD("configure lazy roam request [%d] = %p", id, handle);

 if (roam_param != NULL) {
        params.A_band_boost_threshold  = helper.getIntField(roam_param, "A_band_boost_threshold");
        params.A_band_penalty_threshold  = helper.getIntField(roam_param, "A_band_penalty_threshold");
        params.A_band_boost_factor = helper.getIntField(roam_param, "A_band_boost_factor");
        params.A_band_penalty_factor  = helper.getIntField(roam_param, "A_band_penalty_factor");
        params.A_band_max_boost  = helper.getIntField(roam_param, "A_band_max_boost");
        params.lazy_roam_hysteresis = helper.getIntField(roam_param, "lazy_roam_hysteresis");
        params.alert_roam_rssi_trigger = helper.getIntField(roam_param, "alert_roam_rssi_trigger");
        status = hal_fn.wifi_set_gscan_roam_params(id, handle, &params);
 }
    ALOGE("android_net_wifi_setLazyRoam configured params status=%d\n", status);

 if (status >= 0) {
 int doEnable = enabled ? 1 : 0;
        status = hal_fn.wifi_enable_lazy_roam(id, handle, doEnable);
        ALOGE("android_net_wifi_setLazyRoam enabled roam status=%d\n", status);
 }
 return status >= 0;
}
