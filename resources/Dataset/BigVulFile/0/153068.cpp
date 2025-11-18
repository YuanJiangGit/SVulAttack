bool PDFiumEngine::New(const char* url,
                       const char* headers) {
  url_ = url;
  if (headers)
    headers_ = headers;
  else
    headers_.clear();
  return true;
}
