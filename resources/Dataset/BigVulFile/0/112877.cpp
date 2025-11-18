FilePath GDataCache::GetCacheRootPath(Profile* profile) {
  FilePath cache_base_path;
  chrome::GetUserCacheDirectory(profile->GetPath(), &cache_base_path);
  FilePath cache_root_path =
      cache_base_path.Append(chrome::kGDataCacheDirname);
   return cache_root_path.Append(kGDataCacheVersionDir);
 }
