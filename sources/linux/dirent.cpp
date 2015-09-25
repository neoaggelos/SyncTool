#include "synctool.h"
#include <dirent.h>
#include <unistd.h>
#include <utime.h>

bool isLink(string path)
{
	struct stat st;
	return (lstat(path.c_str(), &st) != -1) && TYPE(st) == S_IFLNK;
}

/* Write with colorized output */
void setColor(string color)
{
	if (gUseColors)
		cout << color;
}

/* File operations */
void createDirectory(string dir)
{
	if (isDirectory(dir))
		return;

	logMessage("MK " + dir, GREEN);
	if (mkdir(dir.c_str(), 0775) == -1)
		die(EXIT_FAILURE, "Error: Could not create directory " + dir);
}

void removeFile(string file)
{
	if (!isFile(file) && !isLink(file))
		return;

	logMessage("RM " + file, RED);
	if(remove(file.c_str()) == -1)
		die(EXIT_FAILURE, "Error: Could not delete " + file);
}

void copyFile(string src, string dst)
{
	if (!filesDiffer(src, dst))
		return;

	if (isFile(dst) || isLink(dst))
		removeFile(dst);

	logMessage("CP " + src + " -> " + dst, BLUE);
	
	ifstream fin(src.c_str(), ios::binary);
	ofstream fout(dst.c_str(), ios::binary);

	if (!(fin.is_open() && fout.is_open()))
		die(EXIT_FAILURE, "Error: Could not copy " + src);

	fout << fin.rdbuf();

	fin.close();
	fout.close();

	struct stat st;

	if (stat(src.c_str(), &st) != -1)
	{
		struct utimbuf buf;

		buf.actime = st.st_atime;
		buf.modtime = st.st_mtime;

		utime(dst.c_str(), &buf);

		/* also copy file permissions */
		chmod(dst.c_str(), st.st_mode);
	}
}

void copyLink(string src, string dst)
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

	target = new char[size+1];
	memset(target, 0, size + 1);

	if (readlink(src.c_str(), target, size) == -1)
		die(EXIT_FAILURE, "Error: Could not read link " + src);

	target[size] = '\0';
	logMessage("LN " + dst + " -> " + target, BLUE);

	if (symlink(target, dst.c_str()) == -1)
		die(EXIT_FAILURE, "Error: Could not create link " + dst);

	delete target;

	if (r != -1)
	{
		struct utimbuf buf;

		buf.actime = st.st_atime;
		buf.modtime = st.st_mtime;

		utime(dst.c_str(), &buf);
	}
}

void removeDirectory(string dir)
{
	if (!isDirectory(dir))
		return;

	DIR *d = opendir(dir.c_str());
	struct dirent *ent = NULL;

	while ((ent = readdir(d)))
	{
		if (string(ent->d_name) == "." || string(ent->d_name) == "..")
			continue;

		string path = dir + '/' + ent->d_name;

		if (isDirectory(path))
			removeDirectory(path);
		else
			removeFile(path);
	}

	closedir(d);

	logMessage("RM " + dir, RED);
	if (rmdir(dir.c_str()) == -1)
		die(EXIT_FAILURE, "Error: Could not delete " + dir);
}

void assertCanOpenDirectory(string dir)
{
	DIR *d = opendir(dir.c_str());
	if (d == NULL)
		die(EXIT_FAILURE, "Error: Cannot access " + dir);

	closedir(d);
}

void copyAllFiles(string src, string dst)
{
	DIR *d = opendir(src.c_str());
	struct dirent *ent = NULL;

	while ((ent = readdir(d)))
	{
		if (string(ent->d_name) == "." || string(ent->d_name) == "..")
			continue;

		string srcPath = src + '/' + ent->d_name;
		string dstPath = dst + '/' + ent->d_name;

		if (isDirectory(srcPath) && !isDirectory(dstPath))
		{
			if (isFile(dstPath) || isLink(dstPath))
				removeFile(dstPath);

			createDirectory(dstPath);
			copyAllFiles(srcPath, dstPath);
		}
		else if (isDirectory(srcPath) && isDirectory(dstPath))
			copyAllFiles(srcPath, dstPath);
		else if (isFile(srcPath))
			copyFile(srcPath, dstPath);
		else if (isLink(srcPath))
			copyLink(srcPath, dstPath);
	}

	closedir(d);
}

void copyNewAndUpdatedFiles(string src, string dst)
{
	DIR *d = opendir(src.c_str());
	struct dirent *ent = NULL;

	while ((ent = readdir(d)))
	{
		if (string(ent->d_name) == "." || string(ent->d_name) == "..")
			continue;

		string srcPath = src + '/' + ent->d_name;
		string dstPath = dst + '/' + ent->d_name;

		if (isDirectory(srcPath) && !isDirectory(dstPath))
		{
			if (isFile(dstPath) || isLink(dstPath))
				logMessage("Warning: " + srcPath + " is a directory, but " + dstPath + " is not.");
			else
			{
				createDirectory(dstPath);
				copyNewAndUpdatedFiles(srcPath, dstPath);
			}
		}
		else if (isDirectory(srcPath) && isDirectory(dstPath))
			copyNewAndUpdatedFiles(srcPath, dstPath);
		else if ((isFile(srcPath) || isLink(srcPath)) && isDirectory(dstPath))
			logMessage("Warning: " + srcPath + " is not a directory, but " + dstPath + " is.");
		else if (isFile(srcPath) && (!isFile(dstPath) || isNewer(srcPath, dstPath)))
			copyFile(srcPath, dstPath);
		else if (isLink(srcPath) && (!isLink(dstPath) || isNewer(srcPath, dstPath)))
			copyLink(srcPath, dstPath);
	}

	closedir(d);
}

void removeMissing(string src, string dst)
{
	DIR *d = opendir(dst.c_str());
	struct dirent *ent;

	while ((ent = readdir(d)))
	{
		if (string(ent->d_name) == "." || string(ent->d_name) == "..")
			continue;

		string srcPath = src + '/' + ent->d_name;
		string dstPath = dst + '/' + ent->d_name;

		if (isDirectory(dstPath) && !isDirectory(srcPath))
			removeDirectory(dstPath);
		else if (isFile(dstPath) && !isFile(srcPath))
			removeFile(dstPath);
		else if (isLink(dstPath) && !isLink(srcPath))
			removeFile(dstPath);
		else if (isDirectory(dstPath) && isDirectory(srcPath))
			removeMissing(srcPath, dstPath);

	}

	closedir(d);
}
