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
		"  -f,   --fast  : Use a faster (but less reliable) method of comparing files\n"
		"     --no-fast  : Disable fast mode\n"
		"  -c,  --color  : Colorize program output\n"
		"    --no-color  : Disable colorized output\n"
		"  -v, --verbose : Output extra progress messages\n"
		"  -m, --mirror  : Use mirror sync mode\n"
		"  -a, --append  : Use append sync mode\n"
	);
	die(EXIT_SUCCESS);
}