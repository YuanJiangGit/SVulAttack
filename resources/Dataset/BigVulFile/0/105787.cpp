const GURL Label::GetURL() const {
  return url_set_ ? url_ : GURL(UTF16ToUTF8(text_));
}
