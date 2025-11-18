void Automation::MouseMove(int tab_id,
                           const gfx::Point& p,
                           Error** error) {
  int windex = 0, tab_index = 0;
  *error = GetIndicesForTab(tab_id, &windex, &tab_index);
  if (*error)
    return;

  std::string error_msg;
  if (!SendMouseMoveJSONRequest(
          automation(), windex, tab_index, p.x(), p.y(), &error_msg)) {
    *error = new Error(kUnknownError, error_msg);
  }
}
