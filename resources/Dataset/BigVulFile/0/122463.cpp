bool HTMLTextAreaElement::valueMissing() const
{
    return willValidate() && valueMissing(value());
}
