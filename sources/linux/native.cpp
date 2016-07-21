#include "synctool.h"

#include <unistd.h>

bool
createDirectory_native(string dir)
{
	return mkdir(dir.c_str(), 0775) != -1;
}

bool
removeFile_native(string file)
{
	return remove(file.c_str()) != -1;
}

bool
copyFile_native(string src, string dst)
{
	ifstream fin(src.c_str(), ios::binary);
	ofstream fout(dst.c_str(), ios::binary);

	if (!(fin.is_open() && fout.is_open()))
		return false;

	fout << fin.rdbuf();

	fin.close();
	fout.close();

	struct stat st;

	if (lstat(src.c_str(), &st) != -1)
	{
		/* also copy file permissions */
		chmod(dst.c_str(), st.st_mode);
	}

	return true;
}

bool
copyLink_native(string src, string dst)
{
	int r;
	char *target;

	int size = 1024;
	struct stat st;

	if (isFile(dst) || isLink(dst))
		removeFile(dst);

	if ((r = lstat(src.c_str(), &st)) != -1)
	{
		size = st.st_size;
	}

	target = new char[size + 1]();

	if (readlink(src.c_str(), target, size) == -1)
		return false;

	target[size] = '\0';

	if (symlink(target, dst.c_str()) == -1)
		return false;

	delete target;
	return true;
}

bool
removeDirectory_native(string dir)
{
	return rmdir(dir.c_str()) != -1;
}

void setColor(string color)
{
	if (gUseColors)
		cout << color;
}

