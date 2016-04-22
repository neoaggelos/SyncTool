SyncTool version 0.3
Developed by Aggelos Kolaitis <neoaggelos@gmail.com>

================================================================================
About SyncTool
================================================================================

SyncTool is meant to be a simple synchronization utility between two local
directories, and it was developed to be used as such. It does not have any
ground-breaking features, not is it meant to have. It's a simple tool that will
get your files from "A" to "B" without much trouble.

SyncTool runs on Windows, Linux and Mac OS X.

================================================================================
Installing SyncTool
================================================================================

You can download SyncTool from the github download page[1] or build it from
source. Refer to BUILD.txt for more information on building from source.

SyncTool is a single executable file, so it does not require any installation.
Just place the executable wherever you feel comfortable and it's ready. It is
highly recommended that you add its location on PATH.

[1]: http://github.com/neoaggelos/synctool/releases/

================================================================================
Using SyncTool
================================================================================

You use SyncTool by opening a command prompt and typing (without the #):

	# synctool.exe [source] [dest] <options>

Options:

 -m,      --mirror : Run in mirror mode. SyncTool will delete all files from
                     [dest] not present in [source] and copy all files from
                     [source] to [dest], overwriting any files in [dest] if
                     needed.

 -a,      --append : Run in append mode. In this mode, no files from [dest] will
                     be deleted. Furthermore, only new and updated files from
                     [source] are synced to [dest].

 -i, --interactive : If [source], [dest] and sync mode are not passed through
                     the command line arguments, ask the user for input.
  --no-interactive : Disable previous option.

 -c,       --color : Colorize program output
        --no-color : Disable previous option

 -f,        --fast : Speeds up file comparison. This option should help speed up
                     the sync of image, video or music files. However, it may
                     not be desired to set this option when syncing critical
                     text files.
         --no-fast : Disable previous option.

 -x, --exclude <P> : Exclude files containing <P> from syncing. Note that <P>
                     is not a wildcard, all files (or folders) containing <P>
                     will be omitted. For example, the correct way to exclude
                     all files from a sub-folder called 'somefolder' is by
                     '-x somefolder' and not '-x somefolder/*'

 -v,     --verbose : Output extra debugging messages in the console.
      --no-verbose : Disable previous option.

================================================================================
Create a script
================================================================================

You can create a small script to automate sync tasks you perform often. To do
so, create a new file with name 'sync.bat' and add the following line in it...

	synctool [source] [dest] <options>

...replacing [source], [dest] and <options> as needed. You can now sync your
files by running this script.

================================================================================
Examples
================================================================================

The following are examples of using SyncTool.

	# synctool.exe C:\\users\\user\\documents e:\\mybackup --append -v
	# synctool.exe c:\\music d:\\music --fast --color --mirror
	# synctool.exe /home/neo/files /media/backup/files --exclude temp
