#include "synctool.h"

void logMessage(string msg, string color)
{
	setColor(color);
	cout << msg << endl;
}

void die(int code, string msg)
{
	setColor(WHITE);
	cout << endl << endl << msg << endl << endl
		<< "Press enter to exit..." << endl;

	(void)getchar();
	exit(code);
}

void printHelp()
{
	logMessage(
		"SyncTool version " VERSION "\n\n"
		"Usage:\n"
		"# synctool [source] [dest] <options>\n\n"
		"Options:\n"
		" -f : Use a faster (but less reliable) method of comparing files\n"
		" -c : Colorize the program messages\n"
		" -v : Write extra progress messages\n"
		" -i : Run in interactive mode\n"
		" -x <name> : Exclude files containing <name>\n"
		" -xx <ext> : Exclude files with extension <ext>\n\n"
		"Sync modes:\n"
		" -m : Mirror mode (default)\n"
		" -a : Append mode\n"
	);
	die(EXIT_FAILURE);
}

bool shouldExclude(string srcPath)
{
	bool ret = false;

	for (list<string>::iterator i = gExcludedTypesList.begin(); i != gExcludedTypesList.end() && !ret; i++) {
		ret = string_endsin(srcPath, *i);
	}

	if (ret) return ret;

	for (list<string>::iterator i = gBlacklist.begin(); i != gBlacklist.end() && !ret; i++) {
		ret = string_contains(srcPath, *i);
	}

	return ret;
}




bool
string_contains(string container, string str)
{
	return container.find(str) != string::npos;
}

bool
string_endsin(string container, string str)
{
	return container.find(str, container.length() - str.length()) != string::npos;
}