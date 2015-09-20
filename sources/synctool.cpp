#include "synctool.h"

void doSync(string src, string dst)
{
	if (gSyncMode == "mirror")
	{
		logMessage("Removing old files...");
		removeMissing(src, dst);

		logMessage("Copying new files...");
		createDirectoryTree(src, dst);
		copyAllFiles(src, dst);

	}
	else if (gSyncMode == "append")
	{
		logMessage("Copying new and updated files...");
		createDirectoryTree(src, dst);
		copyNewAndUpdatedFiles(src, dst);
	}
	else
	{
		die(EXIT_FAILURE, "Error: Unknown sync mode");
	}
}