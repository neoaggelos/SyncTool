#include "synctool.h"

#if defined(OS_WINDOWS)

/* move to native_win.cpp */
#include <Windows.h>

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

bool
createDirectory_native(string dir)
{
	return CreateDirectoryA(dir.c_str(), NULL) == TRUE;
}

bool
removeFile_native(string file)
{
	return DeleteFileA(file.c_str()) == TRUE;
}

bool
copyFile_native(string src, string dst)
{
	return CopyFileA(src.c_str(), dst.c_str(), FALSE) == TRUE;
}

bool
copyLink_native(string src, string dst)
{
	return false;
}

bool
removeDirectory_native(string dir)
{
	return RemoveDirectoryA(dir.c_str()) == TRUE;
}

/*
	Visual Studio also needs the dirent implentation. The implementation comes from
	http://github.com/neoaggelos/vsdirent
*/
#if defined(_MSC_VER)

#include "mydirent.h"

DIR*
opendir(const char* path)
{
	DIR * dir = new DIR();
	dir->ent = new struct dirent();

	/* open handle and read first file */
	string regex = string(path) + "\\*";
	dir->h = FindFirstFileA(regex.c_str(), &(dir->data));

	/* if we fail return NULL */
	if (dir->h == INVALID_HANDLE_VALUE)
	{
		delete dir;
		return NULL;
	}

	/* otherwise, prepare for readdir() */
	dir->ent->d_name = _strdup(dir->data.cFileName);
	dir->readFirstFile = true;

	return dir;
}

struct dirent*
readdir(DIR *dir)
{
	if (dir == NULL)
		return NULL;

	/* this is a hack to workaround how FindFirstFile() works */
	if (dir->readFirstFile)
	{
		dir->readFirstFile = false;
		return dir->ent;
	}

	/* we must read the next file and return it */
	if (FindNextFileA(dir->h, &(dir->data)))
	{
		if (dir->ent->d_name != NULL)
			free((void*)dir->ent->d_name);

		dir->ent->d_name = _strdup(dir->data.cFileName);
		return dir->ent;
	}

	/* no more files */
	return NULL;
}

void
closedir(DIR *dir)
{
	if (dir == NULL)
		return;

	FindClose(dir->h);
	delete dir->ent;
	delete dir;
}

#endif /* defined(_MSC_VER) */

#endif /* defined(OS_WINDOWS) */