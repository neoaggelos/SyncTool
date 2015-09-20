#include "synctool.h"

/* Command line options */
int gUseColors = 1;
int gFastMode = 0;
string gSyncMode = "mirror";

int main(int argc, char** argv)
{
	string src = "", dst = "";

	/* Handle command-line arguments */
	for (int i = 1; i < argc; i++)
	{
		string arg(argv[i]);

		if (arg == "-c" || arg == "--color")
			gUseColors = 1;
		else if (arg == "--no-color")
			gUseColors = 0;
		else if (arg == "-f" || arg == "--fast")
			gFastMode = 1;
		else if (arg == "--no-fast")
			gFastMode = 0;
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
