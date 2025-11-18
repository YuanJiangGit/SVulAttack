static EditorClient& GetEmptyEditorClient() {
  DEFINE_STATIC_LOCAL(EmptyEditorClient, client, ());
  return client;
}
