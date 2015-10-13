/*
	Includes <vsdirent.h> (custom) or <dirent.h>, depending on the compiler.
	vsdirent can be found in: http://github.com/neoaggelos/vsdirent
*/


#if !defined(_MSC_VER)
#include <dirent.h>
#else

#ifndef __vsdirent_h
#define __vsdirent_h

#include <Windows.h>

/* For our implementation we only need ent->d_name */
struct dirent {
	const char *d_name;
};

/* The DIR struct */
typedef struct {
	/* WinAPI specifics */
	HANDLE h;
	WIN32_FIND_DATAA data;

	/* We are responsible for allocating-freeing the dirent memory */
	struct dirent *ent;

	/* This is to not lose file data from FindFirstFile() */
	bool readFirstFile;
} DIR;

/* The well-known readdir() functions */
DIR *opendir(const char* path);
struct dirent* readdir(DIR *dir);
void closedir(DIR* dir);

#endif /* __vsdirent_h */

#endif /* !defined(_MSC_VER) */