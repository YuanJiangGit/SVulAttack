bt_status_t btif_storage_get_adapter_property(bt_property_t *property)
{

 /* Special handling for adapter BD_ADDR and BONDED_DEVICES */
 if (property->type == BT_PROPERTY_BDADDR)
 {
 bt_bdaddr_t *bd_addr = (bt_bdaddr_t*)property->val;
 /* This has been cached in btif. Just fetch it from there */
        memcpy(bd_addr, &btif_local_bd_addr, sizeof(bt_bdaddr_t));
 property->len = sizeof(bt_bdaddr_t);
 return BT_STATUS_SUCCESS;
 }
 else if (property->type == BT_PROPERTY_ADAPTER_BONDED_DEVICES)
 {
 btif_bonded_devices_t bonded_devices;

        btif_in_fetch_bonded_devices(&bonded_devices, 0);

        BTIF_TRACE_DEBUG("%s: Number of bonded devices: %d Property:BT_PROPERTY_ADAPTER_BONDED_DEVICES", __FUNCTION__, bonded_devices.num_devices);

 if (bonded_devices.num_devices > 0)
 {
 property->len = bonded_devices.num_devices * sizeof(bt_bdaddr_t);
            memcpy(property->val, bonded_devices.devices, property->len);
 }

 /* if there are no bonded_devices, then length shall be 0 */
 return BT_STATUS_SUCCESS;
 }
 else if (property->type == BT_PROPERTY_UUIDS)
 {
 /* publish list of local supported services */
 bt_uuid_t *p_uuid = (bt_uuid_t*)property->val;
 uint32_t num_uuids = 0;
 uint32_t i;

        tBTA_SERVICE_MASK service_mask = btif_get_enabled_services_mask();
        LOG_INFO("%s service_mask:0x%x", __FUNCTION__, service_mask);
 for (i=0; i < BTA_MAX_SERVICE_ID; i++)
 {
 /* This should eventually become a function when more services are enabled */
 if (service_mask
 &(tBTA_SERVICE_MASK)(1 << i))
 {
 switch (i)
 {
 case BTA_HFP_SERVICE_ID:
 {
                            uuid16_to_uuid128(UUID_SERVCLASS_AG_HANDSFREE,
                                              p_uuid+num_uuids);
                            num_uuids++;
 }
 /* intentional fall through: Send both BFP & HSP UUIDs if HFP is enabled */
 case BTA_HSP_SERVICE_ID:
 {
                            uuid16_to_uuid128(UUID_SERVCLASS_HEADSET_AUDIO_GATEWAY,
                                              p_uuid+num_uuids);
                            num_uuids++;
 }break;
 case BTA_A2DP_SOURCE_SERVICE_ID:
 {
                            uuid16_to_uuid128(UUID_SERVCLASS_AUDIO_SOURCE,
                                              p_uuid+num_uuids);
                            num_uuids++;
 }break;
 case BTA_HFP_HS_SERVICE_ID:
 {
                            uuid16_to_uuid128(UUID_SERVCLASS_HF_HANDSFREE,
                                              p_uuid+num_uuids);
                            num_uuids++;
 }break;
 }
 }
 }
 property->len = (num_uuids)*sizeof(bt_uuid_t);
 return BT_STATUS_SUCCESS;
 }

 /* fall through for other properties */
 if(!cfg2prop(NULL, property))
 {
 return btif_dm_get_adapter_property(property);
 }
 return BT_STATUS_SUCCESS;
 }
