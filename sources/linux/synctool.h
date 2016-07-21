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
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;

/* Synctool version */
#define VERSION "0.4"

/* Buffer size to use in File IO */
#define BUFFER_SIZE 8192

/* Filetype reported by stat() */
#define TYPE(st) ((st).st_mode & S_IFMT)

/* setColor(RED - GREEN - BLUE - WHITE) */
void setColor(string color);

/* Color macros */
#define RED			"\033[22;31m"
#define GREEN		"\033[22;32m"
#define BLUE		"\033[22;34m"
#define MAGENTA		"\033[22;35m"
#define WHITE 		"\033[01;37m"

/* Utility Functions */
void logMessage(string msg, string color = WHITE, bool if_verbose = false);
void logMessageVerbose(string msg, string color = WHITE);
void die(int code, string msg = "");
void printHelp();
bool string_contains(string container, string str);
bool string_endsin(string container, string str);

/* Low level file operations */
bool isFile(string path);
bool isDirectory(string path);
bool isLink(string path);
bool isNewer(string newFile, string oldFile);
bool filesDiffer(string A, string B);
bool shouldExclude(string srcPath);

/* Low level file operations */
void copyFile(string src, string dst);
void copyLink(string src, string dst);
void createDirectory(string dir);
void removeFile(string file);

/* Native low level file operations */
bool copyFile_native(string src, string dst);
bool copyLink_native(string src, string dst);
bool createDirectory_native(string dir);
bool removeFile_native(string file);
bool removeDirectory_native(string dir);

/* High-level file operations */
void removeDirectory(string dir);
void assertCanOpenDirectory(string dir);
void copyAllFiles(string src, string dst);
void copyNewAndUpdatedFiles(string src, string dst);
void removeMissing(string src, string dst);

/* Super function */
void doSync(string src, string dst);

/* globals that are set from command line options */
extern bool gUseColors;
extern bool gFastMode;
extern bool gVerbose;
extern string gSyncMode;
extern list<string> gBlacklist;
extern list<string> gExcludedTypesList;

#endif /* _synctool_h */
