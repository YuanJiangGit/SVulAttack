static inline bool isObservable(JSTestObj* jsTestObj)
{
    if (jsTestObj->hasCustomProperties())
        return true;
    return false;
}
