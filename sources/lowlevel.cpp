#include "synctool.h"

bool isFile(string path)
{
	struct stat st;
	return (lstat(path.c_str(), &st) != -1) && TYPE(st) == S_IFREG;
}

bool isDirectory(string path)
{
	struct stat st;
	return (lstat(path.c_str(), &st) != -1) && TYPE(st) == S_IFDIR;
}

bool isLink(string path)
{
	struct stat st;
	return (lstat(path.c_str(), &st) != -1) && TYPE(st) == S_IFLNK;
}

bool isNewer(string newFile, string oldFile)
{
	struct stat n, o;
	int rn = lstat(newFile.c_str(), &n);
	int ro = lstat(oldFile.c_str(), &o);

	return (rn != -1) && (ro != -1) && (n.st_mtime > o.st_mtime);
}

bool filesDiffer(string A, string B)
{
	if (gVerbose)
		logMessage("Comparing " + A + " with " + B);

	if (!isFile(A) || !isFile(B))
		return true;

	struct stat stA, stB;

	if (lstat(A.c_str(), &stA) == -1 || lstat(B.c_str(), &stB) == -1)
		return true;

	if (gFastMode)
	{
		return (stA.st_size != stB.st_size);
	}
	else
	{
		if (stA.st_size != stB.st_size)
			return true;

		FILE *fa, *fb;

		if (((fa = fopen(A.c_str(), "rb")) == NULL) || ((fb = fopen(B.c_str(), "rb")) == NULL))
			return true;

		char a[BUFFER_SIZE], b[BUFFER_SIZE];
		int nReadA, nReadB;

		while (((nReadA = fread(a, sizeof(char), BUFFER_SIZE, fa)) != 0) && ((nReadB = fread(b, sizeof(char), BUFFER_SIZE, fb)) != 0))
		{
			if ((nReadA != nReadB) || memcmp(a, b, nReadA) != 0)
			{
				fclose(fa);
				fclose(fb);

				return true;
			}
		}

		fclose(fa);
		fclose(fb);

		return false;
	}
}

/* File operations */
void createDirectory(string dir)
{
	if (isDirectory(dir))
		return;

	logMessage("MK " + dir, GREEN);
	if (!createDirectory_native(dir.c_str()))
		die(EXIT_FAILURE, "Error: Could not create directory " + dir);
}

void removeFile(string file)
{
	if (!isFile(file) && !isLink(file))
		return;

	logMessage("RM " + file, RED);
	if (!removeFile_native(file.c_str()))
		die(EXIT_FAILURE, "Error: Could not delete " + file);
}

void copyFile(string src, string dst)
{
	if (!filesDiffer(src, dst))
		return;

	if (isFile(dst) || isLink(dst))
		removeFile(dst);

	logMessage("CP " + src + " -> " + dst, BLUE);
	if (!copyFile_native(src, dst))
		die(EXIT_FAILURE, "Error: Could not copy " + src);
}

void copyLink(string src, string dst)
{
	logMessage("LN " + src + " -> " + dst, BLUE);

	if (!copyLink_native(src, dst))
		die(EXIT_FAILURE, "Error: Could not create symlink " + dst);
}

extern list<string> gBlacklist;

bool shouldExclude(string srcPath)
{
	bool ret = false;
	for (list<string>::iterator i = gBlacklist.begin(); i != gBlacklist.end() && !ret; i++) {
		ret = srcPath.find(*i) != string::npos;
	}

	return ret;
}