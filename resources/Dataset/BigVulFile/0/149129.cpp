static SQLITE_NOINLINE void *createAggContext(sqlite3_context *p, int nByte){
  Mem *pMem = p->pMem;
  assert( (pMem->flags & MEM_Agg)==0 );
  if( nByte<=0 ){
    sqlite3VdbeMemSetNull(pMem);
    pMem->z = 0;
  }else{
    sqlite3VdbeMemClearAndResize(pMem, nByte);
    pMem->flags = MEM_Agg;
    pMem->u.pDef = p->pFunc;
    if( pMem->z ){
      memset(pMem->z, 0, nByte);
    }
  }
  return (void*)pMem->z;
}
