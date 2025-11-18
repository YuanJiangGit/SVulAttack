cmsBool  Type_MLU_Write(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, void* Ptr, cmsUInt32Number nItems)
{
    cmsMLU* mlu =(cmsMLU*) Ptr;
    cmsUInt32Number HeaderSize;
    cmsUInt32Number  Len, Offset;
    cmsUInt32Number i;

    if (Ptr == NULL) {

          if (!_cmsWriteUInt32Number(io, 0)) return FALSE;
          if (!_cmsWriteUInt32Number(io, 12)) return FALSE;
          return TRUE;
    }

    if (!_cmsWriteUInt32Number(io, mlu ->UsedEntries)) return FALSE;
    if (!_cmsWriteUInt32Number(io, 12)) return FALSE;

    HeaderSize = 12 * mlu ->UsedEntries + sizeof(_cmsTagBase);

    for (i=0; i < mlu ->UsedEntries; i++) {

        Len    =  mlu ->Entries[i].Len;
        Offset =  mlu ->Entries[i].StrW;

        Len    = (Len * sizeof(cmsUInt16Number)) / sizeof(wchar_t);
        Offset = (Offset * sizeof(cmsUInt16Number)) / sizeof(wchar_t) + HeaderSize + 8;

        if (!_cmsWriteUInt16Number(io, mlu ->Entries[i].Language)) return FALSE;
        if (!_cmsWriteUInt16Number(io, mlu ->Entries[i].Country))  return FALSE;
        if (!_cmsWriteUInt32Number(io, Len)) return FALSE;
        if (!_cmsWriteUInt32Number(io, Offset)) return FALSE;
    }

    if (!_cmsWriteWCharArray(io, mlu ->PoolUsed / sizeof(wchar_t), (wchar_t*)  mlu ->MemPool)) return FALSE;

    return TRUE;

    cmsUNUSED_PARAMETER(nItems);
    cmsUNUSED_PARAMETER(self);
}
