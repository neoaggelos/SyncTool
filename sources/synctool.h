/*
	File: synctool.h
	Author: Aggelos Kolaitis <neoaggelos@gmail.com>
	Description: Main header file
*/

#ifndef _synctool_h
#define _synctool_h

#include <iostream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>

#include <cstdio>
#include <cstdlib>

/* i'm bored of writing std::string and std::cout */
using namespace std;

/* program version */
#define VERSION "0.1"

/* program routines */
void printHelp(void);
void doMirrorSync(string src, string dst);
void doAppendSync(string src, string dst);
void doSharedSync(string src, string dst);

/* DIE */
void die(int code);

/* utility functions */
bool fileExists(string file);
bool dirExists(string dir);
bool fileIsNewer(string newFile, string oldFile);

/* common actions using in the different sync modes */
void assert_can_open_directory(string dir);
void remove_missing_files(string src, string dst);
void remove_missing_directories(string src, string dst);
void remove_dir_recursive(string root);
void create_subdirectories(string src, string dst);
void copy_file(string src, string dst);
void copy_all_files(string src, string dst);
void copy_new_and_updated_files(string src, string dst);

#endif /* _synctool_h */
