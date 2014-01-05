/***************
 * NAME:          keyfile.cpp
 * DESCRIPTION:   Keyfile library implementation
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    05.07.97 ah    initial release
 *    26.09.97 mh    PC version
 ***************/

#include <time.h>
#include <string.h>

#ifdef __AMIGA__
#include "keyfile.h"
#pragma libbase KeyfileBase
#include "dos/dos.h"
#include "dos/datetime.h"
#include <pragma/dos_lib.h>
#include <pragma/exec_lib.h>
#endif

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef BRUSH_H
#include "brush.h"
#endif

#ifndef ERROR_H
#include "error.h"
#endif

#ifndef __AMIGA__
#ifndef FILEUTIL_H
#include "fileutil.h"
#endif
typedef char *(*READWRITEPROC)(PICTURE *);
extern char WorkingDirectory[256];
#endif

#ifdef __AMIGA__
#include "rmod:pichand_lib.h"
struct Library *DOSBase;
struct Library *PicHandBase;
#endif

#define FLAGS_SYSTEM_AMIGA 1
#define FLAGS_SYSTEM_PC    2

typedef struct
{
	char name[32];
	ULONG sernum;
	int flags;
} USER;

#include "codekey.h"

#ifdef __AMIGA__
extern "C" void exit(int) { }
#endif

/*************
 * DESCRIPTION:   Test if keyfile is valid. This method compares a coded
 *    user entry with the uncoded ASCII user name and serial ID.
 * INPUT:         none
 * OUTPUT:        FALSE -> keyfile is invalid, TRUE -> keyfile exists/valid
 *************/
extern "C" BOOL keyDecodeKeyfile1()
{
	int i, j;
	ULONG r, c;

#ifdef __AMIGA__
	if(!(user.flags & FLAGS_SYSTEM_AMIGA))
#else
	if(!(user.flags & FLAGS_SYSTEM_PC))
#endif
		return FALSE;

	r = user.sernum;
	for (i = 0; i < 40; i++)
	{
		c = coded_user[i];
		for (j = 0; j < i + 1; j++)
		{
			r = (240995 + 4194301*r) % 2147483647;
			c -= r;
		}
		if(c != (ULONG)user.name[i])
			return FALSE;
	}

	return TRUE;
}

/*************
 * DESCRIPTION:   Test if keyfile is valid. This method compares the coded
 *    generation time and date with the creation time of the keyfile.
 * INPUT:         none
 * OUTPUT:        FALSE if valid, TRUE if invalid
 *************/
#ifdef __AMIGA__
extern "C" BOOL keyDecodeKeyfile2()
{
	BPTR lock;
	FileInfoBlock fib;
	struct DateTime dt;
	int month=12,day=03,year=97;
	int hour=12,minute=32,second=0;
	char date[9], time[9];

	DOSBase = OpenLibrary("dos.library", 0);
	if(!DOSBase)
		return TRUE;

	lock = Lock("S:RayStorm.key", ACCESS_READ);
	if(!lock)
	{
		CloseLibrary(DOSBase);
		return TRUE;
	}

	if(Examine(lock, &fib))
	{
		dt.dat_Format = FORMAT_USA;
		dt.dat_Flags = 0;
		sprintf(date, "%02d-%02d-%02d", month, day, year);
		dt.dat_StrDate = date;
		sprintf(time, "%02d:%02d:%02d", hour, minute, second);
		dt.dat_StrTime = time;
		if(!StrToDate(&dt))
		{
			UnLock(lock);
			CloseLibrary(DOSBase);
			return TRUE;
		}
		// ignore seconds
		fib.fib_Date.ds_Tick = 0;

		if(CompareDates(&fib.fib_Date, &dt.dat_Stamp))
		{
			UnLock(lock);
			CloseLibrary(DOSBase);
			return TRUE;
		}
	}

	UnLock(lock);
	CloseLibrary(DOSBase);

	if(!(user.flags & FLAGS_SYSTEM_AMIGA))
		return TRUE;

	return FALSE;
}
#else  // PC-Version
extern "C" BOOL keyDecodeKeyfile2(char *keyfile)
{
	FILETIME filetime, localfiletime;
	HANDLE hFile;
	WORD date, time;
	SYSTEMTIME stime;
	int month=10,day=25,year=1997;
	int hour=15,minute=16, second=0;

	hFile = CreateFile(keyfile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return TRUE;

	if (!GetFileTime(hFile, &filetime, NULL, NULL))
	{
		CloseHandle(hFile);
		return TRUE;
	}
	CloseHandle(hFile);

	if (!FileTimeToSystemTime(&localfiletime, &stime))
		return TRUE;

	if (stime.wDay != day || stime.wMonth != month || stime.wYear != year)
		return TRUE;

// GetFileTime returns mysterious things...
// if (stime.wMinute != minute || stime.wHour != hour) 
//    return TRUE;

	if(!(user.flags & FLAGS_SYSTEM_PC))
		return TRUE;

	return FALSE;
}
#endif

/*************
 * DESCRIPTION:   Get information about user
 * INPUT:         name     name buffer (32 bytes)
 *                sernum   serial number
 * OUTPUT:        -
 *************/
extern "C" void keyGetInfo(char *name, ULONG *sernum)
{
	strcpy(name, user.name);
	*sernum = user.sernum;
}

/*************
 * DESCRIPTION:   Write an image file.
 * INPUT:         handler     full path of handler-name ("dh0:raystorm/modules/picture/ilbm"(Amiga), "c:\raystorm\modules\picture\ilbm.dll"(PC))
 *                image       image structure
 *                image_error error string (if any)
 * OUTPUT:        FALSE if failed else TRUE
 *************/
extern "C" BOOL keyWritePicture(char *handler, PICTURE *image, char *image_error)
{
	char *errstr;
#ifndef __AMIGA__
	HINSTANCE hPicLib;
	READWRITEPROC picWrite;
#endif

#ifdef __AMIGA__
	PicHandBase = OpenLibrary(handler, 0);
	if(!PicHandBase)
		return FALSE;
#else
	if (!(hPicLib = LoadLibrary(handler)))
		return FALSE;
	if (!(picWrite = (READWRITEPROC)GetProcAddress(hPicLib, "picWrite_")))
		return FALSE;
#endif // __AMIGA__

	// invoke handler
	errstr = picWrite(image);
	if(errstr)
	{
		if(*image_error)
			strcpy(image_error,errstr);
#ifdef __AMIGA__
		CloseLibrary(PicHandBase);
#else
		FreeLibrary(hPicLib);
#endif // __AMIGA__
		return FALSE;
	}
	// free handler
#ifdef __AMIGA__
	CloseLibrary(PicHandBase);
#else
	FreeLibrary(hPicLib);
#endif // __AMIGA__
	return TRUE;
}
