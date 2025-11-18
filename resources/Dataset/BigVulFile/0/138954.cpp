void DeleteWallpaperInList(const std::vector<base::FilePath>& file_list) {
  for (std::vector<base::FilePath>::const_iterator it = file_list.begin();
       it != file_list.end(); ++it) {
    base::FilePath path = *it;
    if (!base::DeleteFile(path, true) &&
        !base::DeleteFile(path.AddExtension(".png"), false)) {
      LOG(ERROR) << "Failed to remove user wallpaper at " << path.value();
    }
  }
}
