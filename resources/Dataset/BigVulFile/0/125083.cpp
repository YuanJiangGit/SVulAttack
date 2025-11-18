GURL GetURLForLayoutTest(const std::string& test_name,
                         FilePath* current_working_directory,
                         bool* enable_pixel_dumping,
                         std::string* expected_pixel_hash) {
  std::string path_or_url = test_name;
  std::string pixel_switch;
  std::string pixel_hash;
  std::string::size_type separator_position = path_or_url.find('\'');
  if (separator_position != std::string::npos) {
    pixel_switch = path_or_url.substr(separator_position + 1);
    path_or_url.erase(separator_position);
  }
  separator_position = pixel_switch.find('\'');
  if (separator_position != std::string::npos) {
    pixel_hash = pixel_switch.substr(separator_position + 1);
    pixel_switch.erase(separator_position);
  }
  if (enable_pixel_dumping) {
    *enable_pixel_dumping =
        (pixel_switch == "--pixel-test" || pixel_switch == "-p");
  }
  if (expected_pixel_hash)
    *expected_pixel_hash = pixel_hash;
  GURL test_url(path_or_url);
  if (!(test_url.is_valid() && test_url.has_scheme())) {
    base::ThreadRestrictions::ScopedAllowIO allow_io;
#if defined(OS_WIN)
    std::wstring wide_path_or_url =
        base::SysNativeMBToWide(path_or_url);
    FilePath local_file(wide_path_or_url);
#else
    FilePath local_file(path_or_url);
#endif
    file_util::AbsolutePath(&local_file);
    test_url = net::FilePathToFileURL(local_file);
  }
  FilePath local_path;
  {
    base::ThreadRestrictions::ScopedAllowIO allow_io;
    if (net::FileURLToFilePath(test_url, &local_path)) {
      file_util::SetCurrentDirectory(local_path.DirName());
    }
    if (current_working_directory)
      file_util::GetCurrentDirectory(current_working_directory);
  }
  return test_url;
}
