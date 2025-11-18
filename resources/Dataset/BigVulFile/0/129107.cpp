bool PermissionsData::CanSilentlyIncreasePermissions(
    const Extension* extension) {
  return extension->location() != Manifest::INTERNAL;
}
