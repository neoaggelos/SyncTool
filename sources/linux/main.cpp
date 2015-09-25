#include "synctool.h"

/* Command line options */
bool gUseColors = true;
bool gFastMode = false;
string gSyncMode = "";

template <typename T>
bool handle_arg(string arg, string name, T& var, T yes)
{
	if (arg == string(string("--") + name) || arg == string(string("-") + name))
	{
		var = yes; return true;
	}
	return false;
}

template <typename T>
bool handle_arg(string arg, string name, T& var, T yes, T no)
{
	if (arg == string(string("--") + name) || arg == string(string("-") + name) || arg == string(string("-") + name.substr(0,1)))
	{
		var = yes; return true;
	}
	else if (arg == string(string("--no-") + name) || arg == string(string("-no-") + name))
	{
		var = no; return true;
	}
	return false;
}

int main(int argc, char** argv)
{
	bool interactive = false;
	string src = "", dst = "";

	/* Handle command-line arguments */
	for (int i = 1; i < argc; i++)
	{
		bool isArg = false;

		isArg |= handle_arg(argv[i], "color",       gUseColors,  true,     false);
		isArg |= handle_arg(argv[i], "fast",        gFastMode,   true,     false);
		isArg |= handle_arg(argv[i], "interactive", interactive, true,     false);

		isArg |= handle_arg<string>(argv[i], "append", gSyncMode, "append");
		isArg |= handle_arg<string>(argv[i], "mirror", gSyncMode, "mirror");

		if (!isArg && ((!isDirectory(argv[i])) || ((src + dst) != "")))
			logMessage("Warning: Unknown option: " + string(argv[i]));
		else if (!isArg && src == "")
			src = argv[i];
		else if (!isArg && dst == "")
			dst = argv[i];
	}

	if (interactive)
	{
		logMessage("Entering interactive mode...");

		if (src == "")
		{
			logMessage("Enter source directory: ");
			cin >> src;
		}

		if (dst == "")
		{
			logMessage("Enter destination directory: ");
			cin >> dst;
		}

		while (gSyncMode != "mirror" && gSyncMode != "append")
		{
			logMessage("Choose sync mode [mirror/append]: ");
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
