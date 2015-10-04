#include "synctool.h"

#include <windows.h>

/* Write with colorized output */
void setColor(string color)
{
	if (gUseColors)
	{
		HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
		if (color == RED)
			SetConsoleTextAttribute(h, WRED);
		else if (color == BLUE)
			SetConsoleTextAttribute(h, WBLUE);
		else if (color == GREEN)
			SetConsoleTextAttribute(h, WGREEN);
		else if (color == WHITE)
			SetConsoleTextAttribute(h, WWHITE);
	}
}

bool isFile(string path)
{
	struct stat st;
	return (stat(path.c_str(), &st) != -1) && TYPE(st) == S_IFREG;
}

bool isDirectory(string path)
{
	struct stat st;
	return (stat(path.c_str(), &st) != -1) && TYPE(st) == S_IFDIR;
}

bool isNewer(string newFile, string oldFile)
{
	struct stat n, o;
	int rn = stat(newFile.c_str(), &n);
	int ro = stat(oldFile.c_str(), &o);

	return (rn != -1) && (ro != -1) && (n.st_mtime > o.st_mtime);
}

bool filesDiffer(string A, string B)
{
	if (gVerbose)
		logMessage("Comparing " + A + " with " + B);

	if (!isFile(A) || !isFile(B))
		return true;

	struct stat stA, stB;

	if (stat(A.c_str(), &stA) == -1 || stat(B.c_str(), &stB) == -1)
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
	if (!CreateDirectoryA(dir.c_str(), NULL))
		die(EXIT_FAILURE, "Error: Could not create directory " + dir);
}

void removeFile(string file)
{
	if (!isFile(file))
		return;

	logMessage("RM " + file, RED);
	if (!DeleteFileA(file.c_str()))
		die(EXIT_FAILURE, "Error: Could not delete " + file);
}

void copyFile(string src, string dst)
{
	if (!filesDiffer(src, dst))
		return;

	removeFile(dst);

	logMessage("CP " + src + " -> " + dst, BLUE);
	if (!CopyFileA(src.c_str(), dst.c_str(), FALSE))
		die(EXIT_FAILURE, "Error: Could not copy " + src);
}

void removeGeneric(string path)
{
	logMessage("RM " + path, RED);
	remove(path.c_str());
}

void removeDirectory(string dir)
{
	if (!isDirectory(dir))
		return;

	HANDLE h = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATAA d;

	string regex = dir + "\\*";
	h = FindFirstFileA(regex.c_str(), &d);
	bool done = (h == INVALID_HANDLE_VALUE);

	while (!done)
	{
		if (string(d.cFileName) != "." && string(d.cFileName) != "..")
		{
			string path = dir + '\\' + d.cFileName;

			if (isDirectory(path))
				removeDirectory(path);
			else if (isFile(path))
				removeFile(path);
			else
				removeGeneric(path);
		}
		done = !FindNextFileA(h, &d);
	}

	FindClose(h);

	logMessage("RM " + dir, RED);
	if (!RemoveDirectoryA(dir.c_str()))
		die(EXIT_FAILURE, "Error: Could not delete " + dir);
}

void assertCanOpenDirectory(string dir)
{
	HANDLE h = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATAA d;

	string regex = dir + "\\*";
	h = FindFirstFileA(regex.c_str(), &d);

	if (h == INVALID_HANDLE_VALUE)
		die(EXIT_FAILURE, "Error: Cannot access " + dir);

	FindClose(h);
}

void copyAllFiles(string src, string dst)
{
	HANDLE h = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATAA d;

	string regex = src + "\\*";
	h = FindFirstFileA(regex.c_str(), &d);

	bool done = (h == INVALID_HANDLE_VALUE);
	while (!done)
	{
		if (string(d.cFileName) != "." && string(d.cFileName) != "..")
		{
			string srcPath = src + '\\' + d.cFileName;
			string dstPath = dst + '\\' + d.cFileName;

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

		done = !FindNextFileA(h, &d);
	}

	FindClose(h);
}

void copyNewAndUpdatedFiles(string src, string dst)
{
	HANDLE h = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATAA d;

	string regex = src + "\\*";
	h = FindFirstFileA(regex.c_str(), &d);

	bool done = (h == INVALID_HANDLE_VALUE);
	while (!done)
	{
		if (string(d.cFileName) != "." && string(d.cFileName) != "..")
		{
			string srcPath = src + '\\' + d.cFileName;
			string dstPath = dst + '\\' + d.cFileName;

			if (isDirectory(srcPath) && !isDirectory(dstPath))
			{
				if (isFile(dstPath))
					logMessage("Warning: " + srcPath + " is a directory, but " + dstPath + " is not.");
				else
				{
					createDirectory(dstPath);
					copyNewAndUpdatedFiles(srcPath, dstPath);
				}
			}
			else if (isDirectory(srcPath) && isDirectory(dstPath))
				copyNewAndUpdatedFiles(srcPath, dstPath);
			else if (isFile(srcPath) && isDirectory(dstPath))
				logMessage("Warning: " + srcPath + " is not a directory, but " + dstPath + " is.");
			else if (isFile(srcPath) && (!isFile(dstPath) || isNewer(srcPath, dstPath)))
				copyFile(srcPath, dstPath);
		}

		done = !FindNextFileA(h, &d);
	}

	FindClose(h);
}

void removeMissing(string src, string dst)
{
	HANDLE h = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATAA d;

	string regex = dst + "\\*";
	h = FindFirstFileA(regex.c_str(), &d);

	bool done = (h == INVALID_HANDLE_VALUE);
	while (!done)
	{
		if (string(d.cFileName) != "." && string(d.cFileName) != "..")
		{
			string srcPath = src + '\\' + d.cFileName;
			string dstPath = dst + '\\' + d.cFileName;

			if (isDirectory(dstPath) && !isDirectory(srcPath))
				removeDirectory(dstPath);
			else if (isFile(dstPath) && !isFile(srcPath))
				removeFile(dstPath);
			else if (isDirectory(dstPath) && isDirectory(srcPath))
				removeMissing(srcPath, dstPath);
		}

		done = !FindNextFileA(h, &d);
	}

	FindClose(h);
}
