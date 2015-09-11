/*
	File: dirent.cpp
	Author: Aggelos Kolaitis <neoaggelos@gmail.com>
	Description: Implements sync commands for the dirent.h interface
*/

#include "synctool.h"
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fstream>

const int MKDIR_MODE = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH;

const string SEP = "/";

bool isFile(string file)
{
	struct stat s;
	int res = stat(file.c_str(), &s);
	return (res != -1 && S_ISREG(s.st_mode));
}

bool isDirectory(string dir)
{
	struct stat s;
	int res = stat(dir.c_str(), &s);
	return (res != -1 && S_ISDIR(s.st_mode));
}

void assert_can_open_directory(string dir)
{
	cout << "Asserting '" << dir << "' exists and is accessible..." << endl;
	DIR *d = opendir(dir.c_str());

	if (d == NULL)
	{
		cout << "Error: Cannot open directory '" << dir << "'" << endl;
		die(EXIT_FAILURE);
	}

	closedir(d);
}

void create_subdirectories(string src, string dst)
{
	DIR *srcdir = opendir(src.c_str());
	struct dirent *ent;

	while( (ent = readdir(srcdir)) )
	{
		if (string(ent->d_name) != "." && string(ent->d_name) != "..")
		{
			if(isDirectory(ent->d_name))
			{
				string newdst = dst + SEP + ent->d_name;
				string newsrc = src + SEP + ent->d_name;
				mkdir(newdst.c_str(), MKDIR_MODE);
				create_subdirectories(newsrc, newdst);
			}
		}
	}

	closedir(srcdir);
}

void remove_missing_files(string src, string dst)
{
	DIR *dstdir = opendir(dst.c_str());
	struct dirent *ent;

	while( (ent = readdir(dstdir)) )
	{
		if (string(ent->d_name) != "." && string(ent->d_name) != "..")
		{
			if (isDirectory(ent->d_name))
			{
				string newsrc = src + SEP + ent->d_name;
				string newdst = dst + SEP + ent->d_name;
				remove_missing_files(newsrc, newdst);
			}
			else
			{
				string fileToCheck = src + SEP + ent->d_name;
				if (!isFile(fileToCheck))
				{
					string fileToRemove = dst + SEP + ent->d_name;
					remove(fileToRemove.c_str());
				}
			}
		}
	}

	closedir(dstdir);
}

void remove_missing_directories(string src, string dst)
{
	DIR *dstdir = opendir(dst.c_str());
	struct dirent *ent;

	while( (ent = readdir(dstdir)) )
	{
		if (string(ent->d_name) != "." && string(ent->d_name) != "..")
		{
			if (isDirectory(ent->d_name))
			{
				string dirToCheck = src + SEP + ent->d_name;
				if (!isDirectory(dirToCheck))
				{
					string dirToRemove = dst + SEP + ent->d_name;
					remove_dir(dirToRemove);
				}
			}
		}
	}

	closedir(dstdir);
}

void remove_dir(string root)
{
	DIR *dir = opendir(root.c_str());
	struct dirent *ent;

	while( (ent = readdir(dir)) )
	{
		if (string(ent->d_name) != "." && string(ent->d_name) != "..")
		{
			if (isDirectory(ent->d_name))
			{
				string newroot = root + SEP + ent->d_name;
				remove_dir(newroot);
			}
			else
			{
				string fileToRemove = root + SEP + ent->d_name;
				remove(fileToRemove.c_str());
			}
		}
	}

	closedir(dir);
	remove(root.c_str());
}

void copy_all_files(string src, string dst)
{
	DIR *srcdir = opendir(src.c_str());
	struct dirent *ent;

	while( (ent = readdir(srcdir)) )
	{
		if (string(ent->d_name) != "." && string(ent->d_name) != "..")
		{
			if (isDirectory(ent->d_name))
			{
				string newsrc = src + SEP + ent->d_name;
				string newdst = dst + SEP + ent->d_name;

				copy_all_files(newsrc, newdst);
			}
			else
			{
				string srcFile = src + SEP + ent->d_name;
				string dstFile = dst + SEP + ent->d_name;

				copyFile(srcFile, dstFile);
			}
		}
	}

	closedir(srcdir);
}

void copy_new_and_updated_files(string src, string dst)
{
	DIR *srcdir = opendir(src.c_str());
	struct dirent *ent;

	while( (ent = readdir(srcdir)) )
	{
		if (string(ent->d_name) != "." && string(ent->d_name) != "..")
		{
			if (isDirectory(ent->d_name))
			{
				string newsrc = src + SEP + ent->d_name;
				string newdst = dst + SEP + ent->d_name;

				copy_new_and_updated_files(newsrc, newdst);
			}
			else
			{
				string srcFile = src + SEP + ent->d_name;
				string dstFile = dst + SEP + ent->d_name;

				copyFileIfNewer(srcFile, dstFile);
			}
		}
	}

	closedir(srcdir);
}

void copy_file_native(string srcFile, string dstFile)
{
	ifstream src(srcFile.c_str(), ios::binary);
	ofstream dst(dstFile.c_str(), ios::binary);

	dst << src.rdbuf();

	/* also copy permissions */
	struct stat st;
	if (stat(srcFile.c_str(), &st) != -1)
	{
		chmod(dstFile.c_str(), st.st_mode);
	}
}
