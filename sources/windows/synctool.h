#ifndef _synctool_h
#define _synctool_h

/* Header files */
#include <iostream>
#include <fstream>
#include <list>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <cwchar>
#include <io.h>
#include <fcntl.h>
#include <Windows.h>
#include "vsdirent.h"

using namespace std;

/* Synctool version */
#define VERSION "0.4"

/* Buffer size to use in File IO */
#define BUFFER_SIZE 8192

/* Filetype reported by stat() */
#define TYPE(st) ((st).st_mode & S_IFMT)

/* setColor(RED - GREEN - BLUE - WHITE) */
void setColor(WORD color);

/* Color macros */
#define RED		    FOREGROUND_INTENSITY|FOREGROUND_RED
#define BLUE		FOREGROUND_INTENSITY|FOREGROUND_BLUE
#define GREEN		FOREGROUND_INTENSITY|FOREGROUND_GREEN
#define MAGENTA	    FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_BLUE
#define WHITE		FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE

/* Utility Functions */
void logMessage(wstring msg, WORD color = WHITE, bool if_verbose = false);
void logMessageVerbose(wstring msg, WORD color = WHITE);
void die(int code, wstring msg = L"");
void printHelp();
bool string_contains(wstring container, wstring str);
bool string_endsin(wstring container, wstring str);

/* Low level file operations */
bool isFile(wstring path);
bool isDirectory(wstring path);
bool isNewer(wstring newFile, wstring oldFile);
bool filesDiffer(wstring A, wstring B);
bool shouldExclude(wstring srcPath);

/* Low level file operations */
void copyFile(wstring src, wstring dst);
void createDirectory(wstring dir);
void removeFile(wstring file);

/* Native low level file operations */
bool copyFile_native(wstring src, wstring dst);
bool createDirectory_native(wstring dir);
bool removeFile_native(wstring file);
bool removeDirectory_native(wstring dir);

/* High-level file operations */
void removeDirectory(wstring dir);
void assertCanOpenDirectory(wstring dir);
void copyAllFiles(wstring src, wstring dst);
void copyNewAndUpdatedFiles(wstring src, wstring dst);
void removeMissing(wstring src, wstring dst);

/* Super function */
void doSync(wstring src, wstring dst);

/* globals that are set from command line options */
extern bool gUseColors;
extern bool gFastMode;
extern bool gVerbose;
extern wstring gSyncMode;
extern list<wstring> gBlacklist;
extern list<wstring> gExcludedTypesList;

#endif /* _synctool_h */
