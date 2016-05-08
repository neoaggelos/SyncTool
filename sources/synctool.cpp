#include "synctool.h"
#include "mydirent.h"

void removeDirectory(string dir)
{
	if (!isDirectory(dir))
		return;

	DIR *d = opendir(dir.c_str());
	struct dirent *ent = NULL;

	while ((ent = readdir(d)))
	{
		if (string(ent->d_name) == "." || string(ent->d_name) == "..")
			continue;

		string path = dir + '/' + ent->d_name;

		if (isDirectory(path))
			removeDirectory(path);
		else
			removeFile(path);
	}

	closedir(d);

	logMessageVerbose("RM " + dir, RED);
	if (!removeDirectory_native(dir.c_str()))
		die(EXIT_FAILURE, "Error: Could not delete " + dir);
}

void assertCanOpenDirectory(string dir)
{
	DIR *d = opendir(dir.c_str());
	if (d == NULL)
		die(EXIT_FAILURE, "Error: Cannot access " + dir);

	closedir(d);
}

void copyAllFiles(string src, string dst)
{
	DIR *d = opendir(src.c_str());
	struct dirent *ent = NULL;

	while ((ent = readdir(d)))
	{
		if (string(ent->d_name) == "." || string(ent->d_name) == "..")
			continue;

		string srcPath = src + '/' + ent->d_name;
		string dstPath = dst + '/' + ent->d_name;

		if (shouldExclude(srcPath)) {
			logMessageVerbose("EX " + srcPath, MAGENTA);

			if (isDirectory(dstPath)) {
				removeDirectory(dstPath);
			}
			else if (isFile(dstPath) || isLink(dstPath)) {
				removeFile(dstPath);
			}

			continue;
		}

		if (isDirectory(srcPath) && !isDirectory(dstPath))
		{
			if (isFile(dstPath) || isLink(dstPath))
				removeFile(dstPath);

			createDirectory(dstPath);
			copyAllFiles(srcPath, dstPath);
		}
		else if (isDirectory(srcPath) && isDirectory(dstPath))
			copyAllFiles(srcPath, dstPath);
		else if (isFile(srcPath))
			copyFile(srcPath, dstPath);
		else if (isLink(srcPath))
			copyLink(srcPath, dstPath);
	}

	closedir(d);
}

void copyNewAndUpdatedFiles(string src, string dst)
{
	DIR *d = opendir(src.c_str());
	struct dirent *ent = NULL;

	while ((ent = readdir(d)))
	{
		if (string(ent->d_name) == "." || string(ent->d_name) == "..")
			continue;

		string srcPath = src + '/' + ent->d_name;
		string dstPath = dst + '/' + ent->d_name;

		if (shouldExclude(srcPath)) {
			logMessageVerbose("EX " + srcPath, MAGENTA);
			continue;
		}
		
		if (isDirectory(srcPath) && !isDirectory(dstPath))
		{
			if (isFile(dstPath) || isLink(dstPath))
				logMessage("Warning: " + srcPath + " is a directory, but " + dstPath + " is not.");
			else
			{
				createDirectory(dstPath);
				copyNewAndUpdatedFiles(srcPath, dstPath);
			}
		}
		else if (isDirectory(srcPath) && isDirectory(dstPath))
			copyNewAndUpdatedFiles(srcPath, dstPath);
		else if ((isFile(srcPath) || isLink(srcPath)) && isDirectory(dstPath))
			logMessage("Warning: " + srcPath + " is not a directory, but " + dstPath + " is.");
		else if (isFile(srcPath) && (!isFile(dstPath) || isNewer(srcPath, dstPath)))
			copyFile(srcPath, dstPath);
		else if (isLink(srcPath) && (!isLink(dstPath) || isNewer(srcPath, dstPath)))
			copyLink(srcPath, dstPath);
	}

	closedir(d);
}

void removeMissing(string src, string dst)
{
	DIR *d = opendir(dst.c_str());
	struct dirent *ent;

	while ((ent = readdir(d)))
	{
		if (string(ent->d_name) == "." || string(ent->d_name) == "..")
			continue;

		string srcPath = src + '/' + ent->d_name;
		string dstPath = dst + '/' + ent->d_name;

		if (isDirectory(dstPath) && !isDirectory(srcPath))
			removeDirectory(dstPath);
		else if (isFile(dstPath) && !isFile(srcPath))
			removeFile(dstPath);
		else if (isLink(dstPath) && !isLink(srcPath))
			removeFile(dstPath);
		else if (isDirectory(dstPath) && isDirectory(srcPath))
			removeMissing(srcPath, dstPath);
	}

	closedir(d);
}

/* Super function */
void doSync(string src, string dst)
{
	if (gSyncMode == "mirror")
	{
		logMessage("Removing old files...");
		removeMissing(src, dst);

		logMessage("Copying new files...");
		copyAllFiles(src, dst);

	}
	else if (gSyncMode == "append")
	{
		logMessage("Copying new and updated files...");
		copyNewAndUpdatedFiles(src, dst);
	}
	else
	{
		die(EXIT_FAILURE, "Error: Unknown sync mode");
	}
}