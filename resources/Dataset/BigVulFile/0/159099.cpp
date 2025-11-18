bool DeleteDownloadedFile(const base::FilePath& path) {
  DCHECK(GetDownloadTaskRunner()->RunsTasksInCurrentSequence());

  if (base::DirectoryExists(path))
    return true;
  return base::DeleteFile(path, false);
}
