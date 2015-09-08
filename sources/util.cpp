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
