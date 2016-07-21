#include "synctool.h"

/* Command line options */
bool gUseColors = true;
bool gFastMode = false;
bool gVerbose = false;
wstring gSyncMode = L"";
list<wstring> gBlacklist;
list<wstring> gExcludedTypesList;

/* Handles command line arguments*/
template <typename T>
inline bool handle_arg(wstring arg, wstring name, wstring shortname, T& var, T yes)
{
	if (arg == wstring(wstring(L"--") + name) || arg == wstring(wstring(L"-") + name) || arg == wstring(wstring(L"-") + shortname))
	{
		var = yes; return true;
	}
	return false;
}

template <typename T>
inline bool handle_arg(wstring arg, wstring name, wstring shortname, T& var, T yes, T no)
{
	if (arg == wstring(wstring(L"--") + name) || arg == wstring(wstring(L"-") + name) || arg == wstring(wstring(L"-") + shortname))
	{
		var = yes; return true;
	}
	else if (arg == wstring(wstring(L"--no-") + name) || arg == wstring(wstring(L"-no-") + name))
	{
		var = no; return true;
	}
	return false;
}

inline bool handle_arg_with_option(int argc, wchar_t *argv[], int &index, wstring name, wstring shortname, wstring &option)
{
	wstring arg = argv[index];
	if (arg == wstring(wstring(L"--") + name) || arg == wstring(wstring(L"-") + name) || arg == wstring(wstring(L"-") + shortname))
	{
		if (index < argc - 1) {
			option = argv[index + 1];
			index++;
			return true;
		}
	}
	return false;
}


//int main(int _unused_argc, char** _unused_argv)
int main()
{
	bool interactive = false;
	wstring src = L"", dst = L"";

    /* get unicode command line arguments */
    int argc;
    wchar_t **argv;
    argv = CommandLineToArgvW(GetCommandLineW(), &argc);

    /* enable unicode output in console */
    _setmode(_fileno(stdout), _O_U16TEXT);

	/* Handle command-line arguments */
	for (int i = 1; i < argc; i++)
	{
		bool isArg = false;
		wstring argOption;

		isArg |= handle_arg_with_option(argc, argv, i, L"exclude", L"x", argOption);
		if (isArg) {
			gBlacklist.push_back(argOption);

			continue;
		}

		isArg |= handle_arg_with_option(argc, argv, i, L"excludetype", L"xx", argOption);
		if (isArg) {
			gExcludedTypesList.push_back(argOption);

			continue;
		}

		isArg |= handle_arg(argv[i], L"color",       L"c", gUseColors,  true, false);
		isArg |= handle_arg(argv[i], L"fast",        L"f", gFastMode,   true, false);
		isArg |= handle_arg(argv[i], L"verbose",     L"v", gVerbose,    true, false);
		isArg |= handle_arg(argv[i], L"interactive", L"i", interactive, true, false);

		isArg |= handle_arg<wstring>(argv[i], L"append", L"a", gSyncMode, L"append");
		isArg |= handle_arg<wstring>(argv[i], L"mirror", L"m", gSyncMode, L"mirror");

		if (!isArg && ((!isDirectory(argv[i])) || ((src != L"") && (dst != L""))))
			logMessage(L"Warning: Unknown option: " + wstring(argv[i]));
		else if (!isArg && src == L"")
			src = argv[i];
		else if (!isArg && dst == L"")
			dst = argv[i];
	}

    /* we allocated argv and it's no longer needed */
    LocalFree(argv);

	if (interactive)
	{
		logMessage(L"Entering interactive mode...");

		if (src == L"")
		{
			logMessage(L"Enter source directory: ");
			wcin >> src;
		}

		if (dst == L"")
		{
			logMessage(L"Enter destination directory: ");
			wcin >> dst;
		}

		while (gSyncMode != L"mirror" && gSyncMode != L"append")
		{
			logMessage(L"Choose sync mode [mirror/append]: ");
			wcin >> gSyncMode;
		}
	}
	else if (gSyncMode == L"")
	{
		gSyncMode = L"mirror";
	}

	if (src == L"" || dst == L"")
	{
		logMessage(L"Error: Source and destination not specified");
		printHelp();
	}

	if (src == dst)
		die(EXIT_FAILURE, L"Error: Source and destination can't be the same");

	assertCanOpenDirectory(src);
	assertCanOpenDirectory(dst);
	doSync(src, dst);

	die(EXIT_SUCCESS, L"Done!");

    return 0;
}
