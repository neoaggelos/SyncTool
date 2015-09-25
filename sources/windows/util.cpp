#include "synctool.h"

void logMessage(string msg, string color)
{
	setColor(color);
	cout << msg << endl;
}

void die(int code, string msg)
{
	setColor(WHITE);
	cout << endl << endl << msg << endl << endl
		<< "Press enter to exit..." << endl;

	(void)getchar();
	exit(code);
}

void printHelp()
{
	logMessage(
		"SyncTool version " VERSION "\n\n"
		"Usage:\n"
		"# synctool [source] [dest] <options>\n\n"
		"Options:\n"
		"  -f,   --fast  : Use a faster (but less reliable) method of comparing files\n"
		"     --no-fast  : Disable fast mode\n"
		"  -c,  --color  : Colorize program output\n"
		"    --no-color  : Disable colorized output\n"
		"  -m, --mirror  : Use mirror sync mode\n"
		"  -a, --append  : Use append sync mode\n"
	);
	die(EXIT_SUCCESS);
}

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

bool isNewer(string newFile, string oldFile)
{
	struct stat n, o;
	int rn = lstat(newFile.c_str(), &n);
	int ro = lstat(oldFile.c_str(), &o);

	return (rn != -1) && (ro != -1) && (n.st_mtime > o.st_mtime);
}

bool filesDiffer(string A, string B)
{
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
