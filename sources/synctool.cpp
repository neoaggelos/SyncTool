/*
	File: synctool.cpp
	Author: Aggelos Kolaitis <neoaggelos@gmail.com>
	Description: Implements the sync mode routines.
*/

#include "synctool.h"

void doMirrorSync(string srcdir, string dstdir)
{
	cout << "Cleaning up old files..." << endl;
	remove_missing_directories(srcdir, dstdir);
	remove_missing_files(srcdir, dstdir);

	cout << "Creating directory tree..." << endl;
	create_subdirectories(srcdir, dstdir);

	cout << "Copying new files..." << endl;
	copy_all_files(srcdir, dstdir);
}

void doAppendSync(string srcdir, string dstdir)
{
	cout << "Creating directory tree..." << endl;
	create_subdirectories(srcdir, dstdir);

	cout << "Copying new files..." << endl;
	copy_new_and_updated_files(srcdir, dstdir);
}

void doSharedSync(string srcdir, string dstdir)
{
	/*
		 Shared sync is actually an append sync back and forth. File conflicts
		 are resolved automatically due to the way append sync works, so the
		 newer files will be kept in both the source and the destination.
	*/
	doAppendSync(srcdir, dstdir);
	doAppendSync(dstdir, srcdir);
}
