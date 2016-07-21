#include "synctool.h"

bool isFile(wstring path)
{
	struct _stat st;
	return (_wstat(path.c_str(), &st) != -1) && TYPE(st) == S_IFREG;
}

bool isDirectory(wstring path)
{
	struct _stat st;
	return (_wstat(path.c_str(), &st) != -1) && TYPE(st) == S_IFDIR;
}

bool isNewer(wstring newFile, wstring oldFile)
{
	struct _stat n, o;
	int rn = _wstat(newFile.c_str(), &n);
	int ro = _wstat(oldFile.c_str(), &o);

	return (rn != -1) && (ro != -1) && (n.st_mtime > o.st_mtime);
}

bool filesDiffer(wstring A, wstring B)
{
	if (!isFile(A) || !isFile(B))
		return true;

	struct _stat stA, stB;

	if (_wstat(A.c_str(), &stA) == -1 || _wstat(B.c_str(), &stB) == -1)
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

		if (((fa = _wfopen(A.c_str(), L"rb")) == NULL) || ((fb = _wfopen(B.c_str(), L"rb")) == NULL))
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
void createDirectory(wstring dir)
{
	if (isDirectory(dir))
		return;

	logMessageVerbose(L"MK " + dir, GREEN);
    if (CreateDirectoryW(dir.c_str(), NULL) != TRUE)
		die(EXIT_FAILURE, L"Error: Could not create directory " + dir);
}

void removeFile(wstring file)
{
	if (!isFile(file))
		return;

	logMessageVerbose(L"RM " + file, RED);
	if (DeleteFileW(file.c_str()) != TRUE)
		die(EXIT_FAILURE, L"Error: Could not delete " + file);
}

void copyFile(wstring src, wstring dst)
{
	if (!filesDiffer(src, dst))
		return;

	if (isFile(dst))
		removeFile(dst);

	logMessageVerbose(L"CP " + src + L" -> " + dst, BLUE);
	if (CopyFileW(src.c_str(), dst.c_str(), FALSE) != TRUE)
		die(EXIT_FAILURE, L"Error: Could not copy " + src);
}