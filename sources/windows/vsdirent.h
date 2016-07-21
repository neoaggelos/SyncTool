/*
	vsdirent can be found in: http://github.com/neoaggelos/vsdirent
*/

#ifndef __vsdirent_h
#define __vsdirent_h

#include <Windows.h>

/* For our implementation we only need ent->d_name */
struct wdirent {
	const wchar_t *d_name;
};

/* The DIR struct */
typedef struct {
	/* WinAPI specifics */
	HANDLE h;
	WIN32_FIND_DATAW data;

	/* We are responsible for allocating-freeing the dirent memory */
	struct wdirent *ent;

	/* This is to not lose file data from FindFirstFile() */
	bool readFirstFile;
} WDIR;

/* The well-known readdir() functions */
WDIR *wopendir(const wchar_t* path);
struct wdirent* wreaddir(WDIR *dir);
void wclosedir(WDIR* dir);

#endif /* __vsdirent_h */