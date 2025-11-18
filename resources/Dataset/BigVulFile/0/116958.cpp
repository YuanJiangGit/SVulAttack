void IndexedDBDispatcher::RequestIDBDatabaseSetVersion(
    const string16& version,
    WebIDBCallbacks* callbacks_ptr,
    int32 idb_database_id,
    WebExceptionCode* ec) {
  ResetCursorPrefetchCaches();
  scoped_ptr<WebIDBCallbacks> callbacks(callbacks_ptr);

  int32 response_id = pending_callbacks_.Add(callbacks.release());
  Send(new IndexedDBHostMsg_DatabaseSetVersion(idb_database_id,
                                               CurrentWorkerId(),
                                               response_id, version, ec));
  if (*ec)
    pending_callbacks_.Remove(response_id);
}
