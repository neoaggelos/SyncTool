#include "synctool.h"

void removeDirectory(wstring dir)
{
	if (!isDirectory(dir))
		return;

	WDIR *d = wopendir(dir.c_str());
	struct wdirent *ent = NULL;

	while ((ent = wreaddir(d)))
	{
		if (wstring(ent->d_name) == L"." || wstring(ent->d_name) == L"..")
			continue;

		wstring path = dir + L'/' + ent->d_name;

		if (isDirectory(path))
			removeDirectory(path);
		else
			removeFile(path);
	}

	wclosedir(d);

	logMessageVerbose(L"RM " + dir, RED);
	if (RemoveDirectoryW(dir.c_str()) != TRUE)
		die(EXIT_FAILURE, L"Error: Could not delete " + dir);
}

void assertCanOpenDirectory(wstring dir)
{
	WDIR *d = wopendir(dir.c_str());
	if (d == NULL)
		die(EXIT_FAILURE, L"Error: Cannot access " + dir);

	wclosedir(d);
}

void copyAllFiles(wstring src, wstring dst)
{
	WDIR *d = wopendir(src.c_str());
	struct wdirent *ent = NULL;

	while ((ent = wreaddir(d)))
	{
		if (wstring(ent->d_name) == L"." || wstring(ent->d_name) == L"..")
			continue;

		wstring srcPath = src + L'/' + ent->d_name;
		wstring dstPath = dst + L'/' + ent->d_name;

		if (shouldExclude(srcPath)) {
			logMessageVerbose(L"EX " + srcPath, MAGENTA);

			if (isDirectory(dstPath)) {
				removeDirectory(dstPath);
			}
			else if (isFile(dstPath)) {
				removeFile(dstPath);
			}

			continue;
		}

		if (isDirectory(srcPath) && !isDirectory(dstPath))
		{
			if (isFile(dstPath))
				removeFile(dstPath);

			createDirectory(dstPath);
			copyAllFiles(srcPath, dstPath);
		}
		else if (isDirectory(srcPath) && isDirectory(dstPath))
			copyAllFiles(srcPath, dstPath);
		else if (isFile(srcPath))
			copyFile(srcPath, dstPath);
	}

	wclosedir(d);
}

void copyNewAndUpdatedFiles(wstring src, wstring dst)
{
	WDIR *d = wopendir(src.c_str());
	struct wdirent *ent = NULL;

	while ((ent = wreaddir(d)))
	{
		if (wstring(ent->d_name) == L"." || wstring(ent->d_name) == L"..")
			continue;

		wstring srcPath = src + L'/' + ent->d_name;
		wstring dstPath = dst + L'/' + ent->d_name;

		if (shouldExclude(srcPath)) {
			logMessageVerbose(L"EX " + srcPath, MAGENTA);
			continue;
		}
		
		if (isDirectory(srcPath) && !isDirectory(dstPath))
		{
			if (isFile(dstPath))
				logMessage(L"Warning: " + srcPath + L" is a directory, but " + dstPath + L" is not.");
			else
			{
				createDirectory(dstPath);
				copyNewAndUpdatedFiles(srcPath, dstPath);
			}
		}
		else if (isDirectory(srcPath) && isDirectory(dstPath))
			copyNewAndUpdatedFiles(srcPath, dstPath);
		else if (isFile(srcPath) && isDirectory(dstPath))
			logMessage(L"Warning: " + srcPath + L" is not a directory, but " + dstPath + L" is.");
		else if (isFile(srcPath) && (!isFile(dstPath) || isNewer(srcPath, dstPath)))
			copyFile(srcPath, dstPath);
	}

	wclosedir(d);
}

void removeMissing(wstring src, wstring dst)
{
	WDIR *d = wopendir(dst.c_str());
	struct wdirent *ent;

	while ((ent = wreaddir(d)))
	{
        if (wstring(ent->d_name) == L"." || wstring(ent->d_name) == L"..")
			continue;

		wstring srcPath = src + L'/' + ent->d_name;
		wstring dstPath = dst + L'/' + ent->d_name;

		if (isDirectory(dstPath) && !isDirectory(srcPath))
			removeDirectory(dstPath);
		else if (isFile(dstPath) && !isFile(srcPath))
			removeFile(dstPath);
		else if (isDirectory(dstPath) && isDirectory(srcPath))
			removeMissing(srcPath, dstPath);
	}

	wclosedir(d);
}

/* Super function */
void doSync(wstring src, wstring dst)
{
	if (gSyncMode == L"mirror")
	{
		logMessage(L"Removing old files...");
		removeMissing(src, dst);

		logMessage(L"Copying new files...");
		copyAllFiles(src, dst);

	}
	else if (gSyncMode == L"append")
	{
		logMessage(L"Copying new and updated files...");
		copyNewAndUpdatedFiles(src, dst);
	}
	else
	{
		die(EXIT_FAILURE, L"Error: Unknown sync mode");
	}
}