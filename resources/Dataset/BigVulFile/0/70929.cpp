cmsBool AllocArray(cmsContext ContextID, _cmsDICarray* a, cmsUInt32Number Count, cmsUInt32Number Length)
{
    memset(a, 0, sizeof(_cmsDICarray));

    if (!AllocElem(ContextID, &a ->Name, Count)) goto Error;
    if (!AllocElem(ContextID, &a ->Value, Count)) goto Error;

    if (Length > 16) {
        if (!AllocElem(ContextID, &a -> DisplayName, Count)) goto Error;

    }
    if (Length > 24) {
        if (!AllocElem(ContextID, &a ->DisplayValue, Count)) goto Error;
    }
    return TRUE;

Error:
    FreeArray(a);
    return FALSE;
}
