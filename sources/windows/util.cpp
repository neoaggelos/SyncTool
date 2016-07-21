#include "synctool.h"

/* Write with colorized output */
void setColor(WORD color)
{
    if (gUseColors)
    {
        HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(h, color);
    }
}

void logMessage(wstring msg, WORD color, bool if_verbose)
{
	if (if_verbose && !gVerbose)
		return;

	setColor(color);
	wcout << msg << endl;
}

void logMessageVerbose(wstring msg, WORD color)
{
	return logMessage(msg, color, true);
}

void die(int code, wstring msg)
{
	setColor(WHITE);
	wcout << endl << endl << msg << endl << endl
		<< L"Press enter to exit..." << endl;

	(void)getchar();
	exit(code);
}

void printHelp()
{
	logMessage(
		L"SyncTool version " VERSION "\n\n"
        L"Usage:\n"
        L"# synctool [source] [dest] <options>\n\n"
        L"Options:\n"
        L" -f : Use a faster (but less reliable) method of comparing files\n"
        L" -c : Colorize the program messages\n"
        L" -v : Write extra progress messages\n"
        L" -i : Run in interactive mode\n"
        L" -x <name> : Exclude files containing <name>\n"
        L" -xx <ext> : Exclude files with extension <ext>\n\n"
        L"Sync modes:\n"
        L" -m : Mirror mode (default)\n"
        L" -a : Append mode\n"
	);
	die(EXIT_FAILURE);
}

bool shouldExclude(wstring srcPath)
{
	bool ret = false;

	for (list<wstring>::iterator i = gExcludedTypesList.begin(); i != gExcludedTypesList.end() && !ret; i++) {
		ret = string_endsin(srcPath, *i);
	}

	if (ret) return ret;

	for (list<wstring>::iterator i = gBlacklist.begin(); i != gBlacklist.end() && !ret; i++) {
		ret = string_contains(srcPath, *i);
	}

	return ret;
}

bool
string_contains(wstring container, wstring str)
{
	return container.find(str) != wstring::npos;
}

bool
string_endsin(wstring container, wstring str)
{
	return container.find(str, container.length() - str.length()) != wstring::npos;
}