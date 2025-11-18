static inline bool isObservable(JSTestEventTarget* jsTestEventTarget)
{
    if (jsTestEventTarget->hasCustomProperties())
        return true;
    if (jsTestEventTarget->impl()->hasEventListeners())
        return true;
    return false;
}
