void TestCreate() {
  int32_t attribs[] = {
      PP_GRAPHICS3DATTRIB_WIDTH, kWidth,
      PP_GRAPHICS3DATTRIB_HEIGHT, kHeight,
      PP_GRAPHICS3DATTRIB_NONE};
  PP_Resource graphics3d_id = PPBGraphics3D()->
      Create(pp_instance(), kInvalidResource, attribs);
  EXPECT(graphics3d_id != kInvalidResource);
  PPBCore()->ReleaseResource(graphics3d_id);
  PP_Resource invalid_graphics3d_id = PPBGraphics3D()->
      Create(0, kInvalidResource, attribs);
  EXPECT(invalid_graphics3d_id == kInvalidResource);
  int32_t empty_attribs[] = {
      PP_GRAPHICS3DATTRIB_NONE};
  PP_Resource graphics3d_empty_attrib_id = PPBGraphics3D()->
      Create(pp_instance(), kInvalidResource, empty_attribs);
  EXPECT(graphics3d_empty_attrib_id != kInvalidResource);
  PPBCore()->ReleaseResource(graphics3d_empty_attrib_id);
  PP_Resource graphics3d_null_attrib_id = PPBGraphics3D()->
      Create(pp_instance(), kInvalidResource, NULL);
  EXPECT(graphics3d_null_attrib_id != kInvalidResource);
  PPBCore()->ReleaseResource(graphics3d_null_attrib_id);
  TEST_PASSED;
}
