/*
	File: main.cpp
	Author: Aggelos Kolaitis <neoaggelos@gmail.com>
	Description: The main() program
*/

#include "synctool.h"

int main(int argc, char** argv)
{
	/* Figure out source and destination directories */
	string src, dst, mode;

	if (argc != 4)
	{
		printHelp();
		die(EXIT_FAILURE);
	}

	/* Set source, destination and sync mode */
	src = argv[1];
	dst = argv[2];

	/* Make sure directories are accessible */
	assert_can_open_directory(src);
	assert_can_open_directory(dst);

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
		<< "Source: '" << src << "'" << endl
		<< "Destination: '" << dst << "'" << endl
		<< "Sync mode: '" << mode << "'" << endl << endl;
#endif

	if (mode == "Mirror")
	{
		doMirrorSync(src, dst);
	}
	else if (mode == "Append")
	{
		doAppendSync(src, dst);
	}
	else if (mode == "Shared")
	{
		doSharedSync(src, dst);
	}

	cout << "Success!" << endl << endl;

	die(EXIT_SUCCESS);
}
