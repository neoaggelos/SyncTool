#ifndef _synctool_h
#define _synctool_h

/* Header files */
#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;

/* Synctool version */
#define VERSION "0.2"

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
#define WHITE 		"\033[01;37m"
#define WRED		FOREGROUND_INTENSITY|FOREGROUND_RED
#define WBLUE		FOREGROUND_INTENSITY|FOREGROUND_BLUE
#define WGREEN		FOREGROUND_INTENSITY|FOREGROUND_GREEN
#define WWHITE		FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE

/* utility functions */
void logMessage(string msg, string color = WHITE);
void die(int code, string msg = "");
void printHelp();
bool isFile(string path);
bool isDirectory(string path);
bool isNewer(string newFile, string oldFile);
bool filesDiffer(string A, string B);

/* File operations */
void copyFile(string src, string dst);
void createDirectory(string dir);
void removeFile(string file);
void removeDirectory(string dir);

/* Native sync operations */
void assertCanOpenDirectory(string dir);
void copyAllFiles(string src, string dst);
void copyNewAndUpdatedFiles(string src, string dst);
void removeMissing(string src, string dst);

/* Super function */
void doSync(string src, string dst);

/* globals that are set from command line options */
extern bool gUseColors;
extern bool gFastMode;
extern string gSyncMode;

#endif /* _synctool_h */
