/*
	File: windows.cpp
	Author: Aggelos Kolaitis <neoaggelos@gmail.com>
	Description: Implements sync commands using the WinAPI
*/

#include "synctool.h"
#include <windows.h>

const string SEP = "\\";

bool isFile(string file)
{
	DWORD res = GetFileAttributesA(file.c_str());
	return res != INVALID_FILE_ATTRIBUTES && !(res & FILE_ATTRIBUTE_DIRECTORY);
}

bool isDirectory(string dir)
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

void create_subdirectories(string src, string dst)
{
	HANDLE hndl = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATAA d;

	string pattern = src + "\\*";

	hndl = FindFirstFileA(pattern.c_str(), &d);
	bool done = (hndl == INVALID_HANDLE_VALUE);
	while (!done)
	{
		if (string(d.cFileName) != "." && string(d.cFileName) != "..")
		{
			if (d.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				string newdst = dst + SEP + d.cFileName;
				string newsrc = src + SEP + d.cFileName;
				CreateDirectoryA(newdst.c_str(), NULL);
				create_subdirectories(newsrc, newdst);
			}
		}

		done = !FindNextFileA(hndl, &d);
	}

	FindClose(hndl);
}

void remove_missing_files(string src, string dst)
{
	HANDLE hndl = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATAA d;

	string pattern = dst + "\\*";

	hndl = FindFirstFileA(pattern.c_str(), &d);
	bool done = (hndl == INVALID_HANDLE_VALUE);

	while (!done)
	{
		if (string(d.cFileName) != "." && string(d.cFileName) != "..")
		{
			if (d.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				string newsrc = src + SEP + d.cFileName;
				string newdst = dst + SEP + d.cFileName;
				remove_missing_files(newsrc, newdst);
			}
			else
			{
				string fileToCheck = src + SEP + d.cFileName;
				if (!isFile(fileToCheck))
				{
					string fileToRemove = dst + SEP + d.cFileName;
					DeleteFileA(fileToRemove.c_str());
				}
			}
		}

		done = !FindNextFileA(hndl, &d);
	}

	FindClose(hndl);
}



void remove_missing_directories(string src, string dst)
{
	HANDLE hndl = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATAA d;

	string pattern = dst + "\\*";

	hndl = FindFirstFileA(pattern.c_str(), &d);
	bool done = (hndl == INVALID_HANDLE_VALUE);

	while (!done)
	{
		if (string(d.cFileName) != "." && string(d.cFileName) != "..")
		{
			if (d.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				string dirToCheck = src + SEP + d.cFileName;
				if (!isDirectory(dirToCheck))
				{
					string dirToRemove = dst + SEP + d.cFileName;
					remove_dir(dirToRemove);
				}
			}
		}

		done = !FindNextFileA(hndl, &d);
	}

	FindClose(hndl);
}

void remove_dir(string root)
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
				remove_dir(newroot);
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

void copy_all_files(string src, string dst)
{
	WIN32_FIND_DATAA d;
	HANDLE hndl = INVALID_HANDLE_VALUE;

	string pattern = src + "\\*";
	hndl = FindFirstFileA(pattern.c_str(), &d);
	bool done = (hndl == INVALID_HANDLE_VALUE);

	while (!done)
	{
		if (string(d.cFileName) != "." && string(d.cFileName) != "..")
		{
			if (d.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				string newsrc = src + SEP + d.cFileName;
				string newdst = dst + SEP + d.cFileName;

				copy_all_files(newsrc, newdst);
			}
			else
			{
				string srcFile = src + SEP + d.cFileName;
				string dstFile = dst + SEP + d.cFileName;

				copyFile(srcFile, dstFile);
			}
		}

		done = !FindNextFileA(hndl, &d);
	}

	FindClose(hndl);
}

void copy_new_and_updated_files(string src, string dst)
{
	WIN32_FIND_DATAA d;
	HANDLE hndl = INVALID_HANDLE_VALUE;

	string pattern = src + "\\*";
	hndl = FindFirstFileA(pattern.c_str(), &d);
	bool done = (hndl == INVALID_HANDLE_VALUE);

	while (!done)
	{
		if (string(d.cFileName) != "." && string(d.cFileName) != "..")
		{
			if (d.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				string newsrc = src + SEP + d.cFileName;
				string newdst = dst + SEP + d.cFileName;

				copy_new_and_updated_files(newsrc, newdst);
			}
			else
			{
				string srcFile = src + SEP + d.cFileName;
				string dstFile = dst + SEP + d.cFileName;

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
