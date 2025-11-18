PassRefPtr<StylePropertySet> CSSComputedStyleDeclaration::makeMutable()
{
    return copy();
}
