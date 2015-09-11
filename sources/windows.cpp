/*
	File: windows.cpp
	Author: Aggelos Kolaitis <neoaggelos@gmail.com>
	Description: Implements sync commands using the WinAPI
*/

#include "synctool.h"
#include <windows.h>

const string SEP = "\\";

bool fileExists(string file)
{
	DWORD res = GetFileAttributesA(file.c_str());
	return res != INVALID_FILE_ATTRIBUTES && !(res & FILE_ATTRIBUTE_DIRECTORY);
}

bool dirExists(string dir)
{
	DWORD res = GetFileAttributesA(dir.c_str());
	return res != INVALID_FILE_ATTRIBUTES && (res & FILE_ATTRIBUTE_DIRECTORY);
}

void assert_can_open_directory(string dir)
{
	cout << "Asserting '" << dir << "' exists and is accessible..." << endl;
	HANDLE hndl = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATAA d;

	string pattern = dir + "\\*";

	hndl = FindFirstFileA(pattern.c_str(), &d);

	if (hndl == INVALID_HANDLE_VALUE)
	{
		cout << "Error: Cannot open directory '" << dir << "'" << endl;
		die(EXIT_FAILURE);
	}
}

void create_subdirectories(string srcdir, string dstdir)
{
	HANDLE hndl = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATAA d;

	string pattern = srcdir + "\\*";

	hndl = FindFirstFileA(pattern.c_str(), &d);
	bool done = (hndl == INVALID_HANDLE_VALUE);
	while (!done)
	{
		if (string(d.cFileName) != "." && string(d.cFileName) != "..")
		{
			if (d.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				string newdst = dstdir + SEP + d.cFileName;
				string newsrc = srcdir + SEP + d.cFileName;
				CreateDirectoryA(newdst.c_str(), NULL);
				create_subdirectories(newsrc, newdst);
			}
		}

		done = !FindNextFileA(hndl, &d);
	}

	FindClose(hndl);
}

void remove_missing_files(string srcdir, string dstdir)
{
	HANDLE hndl = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATAA d;

	string pattern = dstdir + "\\*";

	hndl = FindFirstFileA(pattern.c_str(), &d);
	bool done = (hndl == INVALID_HANDLE_VALUE);

	while (!done)
	{
		if (string(d.cFileName) != "." && string(d.cFileName) != "..")
		{
			if (d.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				string newsrc = srcdir + SEP + d.cFileName;
				string newdst = dstdir + SEP + d.cFileName;
				remove_missing_files(newsrc, newdst);
			}
			else
			{
				string fileToCheck = srcdir + SEP + d.cFileName;
				if (!fileExists(fileToCheck))
				{
					string fileToRemove = dstdir + SEP + d.cFileName;
					DeleteFileA(fileToRemove.c_str());
				}
			}
		}

		done = !FindNextFileA(hndl, &d);
	}

	FindClose(hndl);
}

void remove_missing_directories(string srcdir, string dstdir)
{
	HANDLE hndl = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATAA d;

	string pattern = dstdir + "\\*";

	hndl = FindFirstFileA(pattern.c_str(), &d);
	bool done = (hndl == INVALID_HANDLE_VALUE);

	while (!done)
	{
		if (string(d.cFileName) != "." && string(d.cFileName) != "..")
		{
			if (d.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				string dirToCheck = srcdir + SEP + d.cFileName;
				if (!dirExists(dirToCheck))
				{
					string dirToDelete = dstdir + SEP + d.cFileName;
					remove_dir_recursive(dirToDelete);
				}
			}
		}

		done = !FindNextFileA(hndl, &d);
	}

	FindClose(hndl);
}

void remove_dir_recursive(string root)
{
	WIN32_FIND_DATAA d;
	HANDLE hndl = INVALID_HANDLE_VALUE;
	string pattern = root + "\\*";

	hndl = FindFirstFileA(pattern.c_str(), &d);
	bool done = (hndl == INVALID_HANDLE_VALUE);

	while (!done)
	{
		if (string(d.cFileName) != "." && string(d.cFileName) != "..")
		{
			if (d.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				string newroot = root + SEP + d.cFileName;
				remove_dir_recursive(newroot);
			}
			else
			{
				string fileToRemove = root + SEP + d.cFileName;
				DeleteFileA(fileToRemove.c_str());
			}

		}

		done = !FindNextFileA(hndl, &d);
	}

	FindClose(hndl);
	RemoveDirectoryA(root.c_str());
}

void copy_all_files(string srcdir, string dstdir)
{
	WIN32_FIND_DATAA d;
	HANDLE hndl = INVALID_HANDLE_VALUE;

	string pattern = srcdir + "\\*";
	hndl = FindFirstFileA(pattern.c_str(), &d);
	bool done = (hndl == INVALID_HANDLE_VALUE);

	while (!done)
	{
		if (string(d.cFileName) != "." && string(d.cFileName) != "..")
		{
			if (d.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				string newsrc = srcdir + SEP + d.cFileName;
				string newdst = dstdir + SEP + d.cFileName;

				copy_all_files(newsrc, newdst);
			}
			else
			{
				string srcFile = srcdir + SEP + d.cFileName;
				string dstFile = dstdir + SEP + d.cFileName;

				copyFile(srcFile, dstFile);
			}
		}

		done = !FindNextFileA(hndl, &d);
	}

	FindClose(hndl);
}

void copy_new_and_updated_files(string srcdir, string dstdir)
{
	WIN32_FIND_DATAA d;
	HANDLE hndl = INVALID_HANDLE_VALUE;

	string pattern = srcdir + "\\*";
	hndl = FindFirstFileA(pattern.c_str(), &d);
	bool done = (hndl == INVALID_HANDLE_VALUE);

	while (!done)
	{
		if (string(d.cFileName) != "." && string(d.cFileName) != "..")
		{
			if (d.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				string newsrc = srcdir + SEP + d.cFileName;
				string newdst = dstdir + SEP + d.cFileName;

				copy_new_and_updated_files(newsrc, newdst);
			}
			else
			{
				string srcFile = srcdir + SEP + d.cFileName;
				string dstFile = dstdir + SEP + d.cFileName;

				copyFileIfNewer(srcFile, dstFile);
			}
		}

		done = !FindNextFileA(hndl, &d);
	}

	FindClose(hndl);
}

void copy_file_native(string srcFile, string dstFile)
{
	DeleteFileA(dstFile.c_str());
	CopyFileA(srcFile.c_str(), dstFile.c_str(), FALSE);
}
