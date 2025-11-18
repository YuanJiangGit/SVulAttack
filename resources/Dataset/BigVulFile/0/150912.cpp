void FakeCentral::RemoveFakeDescriptor(const std::string& descriptor_id,
                                       const std::string& characteristic_id,
                                       const std::string& service_id,
                                       const std::string& peripheral_address,
                                       RemoveFakeDescriptorCallback callback) {
  FakeRemoteGattCharacteristic* fake_remote_gatt_characteristic =
      GetFakeRemoteGattCharacteristic(peripheral_address, service_id,
                                      characteristic_id);
  if (!fake_remote_gatt_characteristic) {
    std::move(callback).Run(false);
    return;
  }

  std::move(callback).Run(
      fake_remote_gatt_characteristic->RemoveFakeDescriptor(descriptor_id));
}
