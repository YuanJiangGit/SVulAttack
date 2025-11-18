bool MockPrinter::GetBitmapChecksum(
    unsigned int page, std::string* checksum) const {
  if (printer_status_ != PRINTER_READY || page >= pages_.size())
    return false;
  *checksum = pages_[page]->image().checksum();
  return true;
}
