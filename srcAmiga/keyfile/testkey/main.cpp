#include <stdlib.h>
#include <stdio.h>

#include <pragma/exec_lib.h>

#include "dos/dos.h"
#include "dos/datetime.h"
#include <pragma/dos_lib.h>

typedef struct
{
	UBYTE r,g,b, flag;
} SMALL_COLOR;

#include "ray:keyfile/keyfile_lib.h"

struct Library *KeyfileBase;

#define FLAGS_SYSTEM_AMIGA 1
#define FLAGS_SYSTEM_PC    2

typedef struct
{
	char name[32];
	ULONG sernum;
	int flags;
} USER;

#include "ray:keyfile/codekey.h"

// this variable must be set to a value,
// so that the CRC of the compiled
// library reaches a given value
static UBYTE crc_value = 0;

/*************
 * DESCRIPTION:   Test if keyfile is valid. This method compares a coded
 *    user entry with the uncoded ASCII user name an serial ID.
 * INPUT:         none
 * OUTPUT:        FALSE -> keyfile is invalid, TRUE -> keyfile exists/valid
 *************/
BOOL _keyDecodeKeyfile1()
{
	int i, j;
	ULONG r, c;

	for (i = 0; i < 40; i++)
	{
		c = coded_user[i];
#ifdef __SWAP__
		c = Swap(c);
#endif
		for (j = 0; j < i + 1; j++)
		{
			r = (240995 + 4194301*r) % 2147483647;
			c -= r;
		}
		if(c != user.name[i])
			return FALSE;
	}

	return TRUE;
}

BOOL _keyDecodeKeyfile2()
{
	BPTR lock;
	FileInfoBlock fib;
	struct DateTime dt;
	int month=9,day=11,year=97;
	int hour=21,minute=41,second=0;
	char date[9], time[9];

	lock = Lock("S:RayStorm.key", ACCESS_READ);
	if(!lock)
	{
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
			return TRUE;
		}
		// ignore seconds
		fib.fib_Date.ds_Tick = 0;

		if(CompareDates(&fib.fib_Date, &dt.dat_Stamp))
		{
			UnLock(lock);
			return TRUE;
		}
	}

	UnLock(lock);

	return FALSE;
}

void main()
{
	ULONG sernum;
	char name[256];

	KeyfileBase = OpenLibrary("S:RayStorm.key", 0);
	if(!KeyfileBase)
	{
		printf("Can't open keyfile\n");
		exit(1);
	}

	keyGetInfo(name, &sernum);
	printf("User:   %s\nSerial: %d\n", name, sernum);

	if(keyDecodeKeyfile1())
		printf("Test1: valid\n");
	else
		printf("Test1: invalid\n");

	if(keyDecodeKeyfile2())
		printf("Test2: invalid\n");
	else
		printf("Test2: valid\n");

	CloseLibrary(KeyfileBase);
}