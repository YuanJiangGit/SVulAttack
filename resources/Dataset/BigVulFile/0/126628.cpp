int TabStripModel::GetIndexOfWebContents(const WebContents* contents) const {
  for (size_t i = 0; i < contents_data_.size(); ++i) {
    if (contents_data_[i]->contents == contents)
      return i;
  }
  return kNoTab;
}
