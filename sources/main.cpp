/*
	File: main.cpp
	Author: Aggelos Kolaitis <neoaggelos@gmail.com>
	Description: The main() program
*/

#include "synctool.h"

int main(int argc, char** argv)
{
	/* Figure out source and destination directories */
	string srcdir, dstdir, mode;

	if (argc != 4)
	{
		printHelp();
		die(EXIT_FAILURE);
	}

	/* Set source, destination and sync mode */
	srcdir = argv[1];
	dstdir = argv[2];

	/* Make sure directories are accessible */
	assert_can_open_directory(srcdir);
	assert_can_open_directory(dstdir);

	/* Set sync mode */
	if (string(argv[3]) == "--mirror" || string(argv[3]) == "-m")
	{
		mode = "Mirror";
	}
	else if (string(argv[3]) == "--append" || string(argv[3]) == "-a")
	{
		mode = "Append";
	}
	else if (string(argv[3]) == "--shared" || string(argv[3]) == "-s")
	{
		mode = "Shared";
	}
	else
	{
		cout << "Error: Unknown sync mode: '" << argv[3] << "'" << endl << endl;
		printHelp();
		die(EXIT_FAILURE);
	}

#ifdef _DEBUG
	cout << "SyncTool version: " << VERSION << endl << endl
		<< "Source: '" << srcdir << "'" << endl
		<< "Destination: '" << dstdir << "'" << endl
		<< "Sync mode: '" << mode << "'" << endl << endl;
#endif

	if (mode == "Mirror")
	{
		doMirrorSync(srcdir, dstdir);
	}
	else if (mode == "Append")
	{
		doAppendSync(srcdir, dstdir);
	}
	else if (mode == "Shared")
	{
		doSharedSync(srcdir, dstdir);
	}

	cout << "Success!" << endl << endl;

	die(EXIT_SUCCESS);
}
