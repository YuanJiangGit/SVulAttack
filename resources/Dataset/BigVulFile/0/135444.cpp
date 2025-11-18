void Document::write(const String& text, Document* enteredDocument, ExceptionState& exceptionState)
{
    write(SegmentedString(text), enteredDocument, exceptionState);
}
