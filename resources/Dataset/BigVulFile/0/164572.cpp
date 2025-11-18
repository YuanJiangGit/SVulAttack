static int dbpageEof(sqlite3_vtab_cursor *pCursor){
  DbpageCursor *pCsr = (DbpageCursor *)pCursor;
  return pCsr->pgno > pCsr->mxPgno;
}
