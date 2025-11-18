device_luks_unlock_authorized_cb (Daemon *daemon,
                                  Device *device,
                                  DBusGMethodInvocation *context,
                                  const gchar *action_id,
                                  guint num_user_data,
                                  gpointer *user_data_elements)
{
  const char *secret = user_data_elements[0];
  char **options = user_data_elements[1];
  device_luks_unlock_internal (device, secret, options, NULL, NULL, context);
}
