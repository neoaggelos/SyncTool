SyncTool version 0.1
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

You can download SyncTool from the github download page [1] or build it from
source. Refer to BUILD.txt for more information on building from source.

SyncTool is a single executable file, so it does not require any installation.
Just place the executable wherever you feel comfortable and it's ready. It is
highly recommended that you add its location on PATH.

[1]: http://github.com/neoaggelos/synctool/releases/tag/v0.1

================================================================================
Using SyncTool
================================================================================

You use SyncTool by opening a command prompt and typing (without the #):

	# synctool.exe [source] [destination] [--mode]

Replace [source] and [destination] with the source and destination directories.
Replace [--mode] with the desired sync mode. See USING.txt for details.

Alternatively, create a batch (or shell for Linux) script and put the following
text in it:

	synctool [source] [destination] [--mode]

Save it as 'sync.bat' (or 'sync.sh' for Linux) and put in your desktop. Every
time you want to sync, just double-click the script. You can also use a task
scheduler to automatically call the script. Just make sure you don't mess with
the directories while synctool is running, as it may make this messy.