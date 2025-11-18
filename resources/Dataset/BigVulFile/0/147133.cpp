void DocumentLoader::ReplaceDocumentWhileExecutingJavaScriptURL(
    const KURL& url,
    Document* owner_document,
    bool should_reuse_default_view,
    const String& source) {
  InstallNewDocument(url, owner_document, should_reuse_default_view, MimeType(),
                     writer_ ? writer_->Encoding() : g_empty_atom,
                     InstallNewDocumentReason::kJavascriptURL,
                     kForceSynchronousParsing, NullURL());
  if (!source.IsNull())
    writer_->AppendReplacingData(source);
  EndWriting();
}
