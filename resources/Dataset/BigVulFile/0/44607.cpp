static int mount_entry_create_dir_file(const struct mntent *mntent,
				       const char* path)
{
	char *pathdirname = NULL;
	int ret = 0;
	FILE *pathfile = NULL;

	if (hasmntopt(mntent, "create=dir")) {
		if (mkdir_p(path, 0755) < 0) {
			WARN("Failed to create mount target '%s'", path);
			ret = -1;
		}
	}

	if (hasmntopt(mntent, "create=file") && access(path, F_OK)) {
		pathdirname = strdup(path);
		pathdirname = dirname(pathdirname);
		if (mkdir_p(pathdirname, 0755) < 0) {
			WARN("Failed to create target directory");
		}
		pathfile = fopen(path, "wb");
		if (!pathfile) {
			WARN("Failed to create mount target '%s'", path);
			ret = -1;
		}
		else
			fclose(pathfile);
	}
	free(pathdirname);
	return ret;
 }
