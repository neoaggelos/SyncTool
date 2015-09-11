/*
	File: util.cpp
	Author: Aggelos Kolaitis <neoaggelos@gmail.com>
	Description: Implements common utility functions
*/

#include "synctool.h"

void printHelp()
{
	cout << "SyncTool version: " << VERSION << endl << endl
		<< "Usage:" << endl
		<< "$ synctool [source] [destination] [mode]" << endl
		<< "$ synctool --help" << endl << endl
		<< "Available sync modes:" << endl
		<< "  --mirror,  -m" << endl
		<< "  --append,  -a" << endl
		<< "  --shared,  -s" << endl << endl;
}

void die(int code)
{
	cout << endl << "Press enter to quit..." << endl;
	(void) getchar();
	exit(code);
}

bool fileIsNewer(string newFile, string oldFile)
{
	struct stat o, n;
	stat(oldFile.c_str(), &o);
	stat(newFile.c_str(), &n);

	/* it's newer when it's modified last, thus its mtime is bigger */
	return n.st_mtime > o.st_mtime;
}

bool filesDiffer(string a, string b)
{
	struct stat as, bs;

	/* if size is different, files cant be the same */
	if (stat(a.c_str(), &as) != -1 && stat(b.c_str(), &bs) != -1 && as.st_size != bs.st_size)
		return true;

	ifstream af(a, ios::binary);
	ifstream bf(b, ios::binary);

	char ach, bch;

	do {
		/* if both files reach EOF together then they are the same */
		if (!af.good() && !bf.good())
			return false;

		/* if one files reaches EOF and the other doesn't they differ */
		if ((af.good() && !bf.good()) || (!af.good() && bf.good()))
			return true;

		af.get(ach);
		bf.get(bch);
	} while (ach != bch);

	return true;
}

void copyFile(string srcFile, string dstFile)
{
	if (filesDiffer(srcFile, dstFile))
	{
		copy_file_native(srcFile, dstFile);
	}

	/* update modification time to that of the source file */
	struct stat s;
	if (stat(srcFile.c_str(), &s) != -1)
	{
		struct utimbuf newTime;
		newTime.actime = s.st_atime;
		newTime.modtime = s.st_mtime;
		utime(dstFile.c_str(), &newTime);
	}
}

void copyFileIfNewer(string srcFile, string dstFile)
{
	if (fileIsNewer(srcFile, dstFile))
	{
		copyFile(srcFile, dstFile);
	}
}
