#include "synctool.h"
#include <dirent.h>
#include <unistd.h>
#include <utime.h>

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
void removeFile(string file)
{
	if (!isFile(file))
		return;

	logMessage("RM " + file, RED);
	if(remove(file.c_str()) == -1)
		die(EXIT_FAILURE, "Error: Could not delete " + file);
}

void copyFile(string src, string dst)
{
	if (!filesDiffer(src, dst))
		return;

	if (isFile(dst))
		removeFile(dst);

	logMessage("CP " + src + " -> " + dst, BLUE);
	
	ifstream fin(src.c_str(), ios::binary);
	ifstream fout(dst.c_str(), ios::binary);

	if (!(fin.is_open() && fout.is_open() && (fout << fin.rdbuf())))
		die(EXIT_FAILURE, "Error: Could not copy " + src);

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

	removeFile(dst);
	
	if ((r = lstat(src.c_str(), &st)) != -1)
	{
		size = st.st_size;
	}

	target = new char[size];

	if (readlink(src.c_str(), target, size) == -1)
		die(EXIT_FAILURE, "Error: Could not read link " + src);

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

		if (isDirectory(srcPath))
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
			logMessage("Warning: " + srcPath + " is a directory, but " + dstPath + " is not.");
		else if (!isDirectory(srcPath) && isDirectory(dstPath))
			logMessage("Warning: " + srcPath + " is not a directory, but " + dstPath + " is.");
		else if (isDirectory(srcPath))
			copyNewAndUpdatedFiles(srcPath, dstPath);
		else if (isFile(srcPath) && isNewer(srcPath, dstPath))
			copyFile(srcPath, dstPath);
		else if (isLink(srcPath))
			copyLink(srcPath, dstPath);
	}

	closedir(d);
}

void createDirectoryTree(string src, string dst)
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

			logMessage("MK " + dstPath);
			if (mkdir(dstPath.c_str(), 0775) == -1)
				die(EXIT_FAILURE, "Could not create " + dstPath);

			struct stat st;

			if (stat(srcPath.c_str(), &st) != -1)
			{
				struct utimbuf buf;
				buf.actime = st.st_atime;
				buf.modtime = st.st_mtime;

				utime(dstPath.c_str(), &buf);
			}
		}

		if (isDirectory(srcPath) && isDirectory(dstPath))
			createDirectoryTree(srcPath, dstPath);
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
