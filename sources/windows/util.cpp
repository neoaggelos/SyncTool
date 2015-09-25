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
		"  -m, --mirror  : Use mirror sync mode\n"
		"  -a, --append  : Use append sync mode\n"
	);
	die(EXIT_SUCCESS);
}

/* Write with colorized output */
void setColor(string color)
{
	if (gUseColors)
	{
		HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
		if (color == RED)
			SetConsoleTextAttribute(h, WRED);
		else if (color == BLUE)
			SetConsoleTextAttribute(h, WBLUE);
		else if (color == GREEN)
			SetConsoleTextAttribute(h, WGREEN);
		else if (color == WHITE)
			SetConsoleTextAttribute(h, WWHITE);
	}
}
