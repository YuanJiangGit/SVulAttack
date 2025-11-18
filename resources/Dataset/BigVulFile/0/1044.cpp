void GfxDeviceGrayColorSpace::getGray(GfxColor *color, GfxGray *gray) {
  *gray = clip01(color->c[0]);
}
