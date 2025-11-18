const AtomicString& Document::referrer() const
{
    if (loader())
        return loader()->request().httpReferrer();
    return nullAtom;
}
