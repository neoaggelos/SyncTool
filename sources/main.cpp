#include "synctool.h"

/* Command line options */
bool gUseColors = true;
bool gFastMode = false;
string gSyncMode = "";

int main(int argc, char** argv)
{
	bool interactiveMode = false;
	string src = "", dst = "";

	/* Handle command-line arguments */
	for (int i = 1; i < argc; i++)
	{
		string arg(argv[i]);

		if (arg == "-c" || arg == "--color")
			gUseColors = true;
		else if (arg == "--no-color")
			gUseColors = false;
		else if (arg == "-f" || arg == "--fast")
			gFastMode = true;
		else if (arg == "--no-fast")
			gFastMode = false;
		else if (arg == "-i" || arg == "--interactive")
			interactiveMode = true;
		else if (arg == "--no-interactive")
			interactiveMode = false;
		else if (arg == "-m" || arg == "--mirror")
			gSyncMode = "mirror";
		else if (arg == "-a" || arg == "--append")
			gSyncMode = "append";
		else if (!isDirectory(arg) || (src != "" && dst != ""))
			logMessage("Warning: Unknown option " + arg);
		else if (src == "")
			src = arg;
        else if (dst == "")
            dst = arg;
	}

	if (interactiveMode)
	{
		while (src == "" || !isDirectory(src))
		{
			logMessage("Enter source directory: ");
			cin >> src;
		}

		while (dst == "" || !isDirectory(dst))
		{
			logMessage("Enter destination directory: ");
			cin >> dst;
		}

		while (gSyncMode != "mirror" && gSyncMode != "append")
		{
			logMessage("Choose sync mode: [mirror/append]: ");
			cin >> gSyncMode;
		}
	}
	else if (gSyncMode == "")
	{
		gSyncMode = "mirror";
	}

	if (src == "" || dst == "")
	{
		logMessage("Error: Source and destination not specified");
		printHelp();
	}

	if (src == dst)
		die(EXIT_FAILURE, "Error: Source and destination can't be the same");

	assertCanOpenDirectory(src);
	assertCanOpenDirectory(dst);
	doSync(src, dst);

	die(EXIT_SUCCESS, "Done!");
}
