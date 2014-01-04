/***************
 * PROGRAM:       codekey
 * NAME:          codekey.cpp
 * DESCRIPTION:   main module for keyfile coder
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    29.01.97 ah    initial release
 ***************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "typedefs.h"

#define CATCOMP_NUMBERS
#define CATCOMP_STRINGS

enum
{
	AMIGA,PC
};

typedef unsigned char UBYTE;

static char *Computer[] =
{
	"AMIGA",
	"PC",
	NULL
};

static char *keyfile_types[] =
{
	"Old",
	"New",
	NULL
};

static UBYTE data[33] =
{
	'K','E','Y', 0 ,
	 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
	 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
	 0 , 0 , 0 , 0 , 0 ,
	 0 , 0 , 0 , 0
};

typedef struct
{
	char name[32];
	ULONG sernum;
	int flags;
} USER;

#define FLAGS_SYSTEM_AMIGA 1
#define FLAGS_SYSTEM_PC    2

#ifndef MAKE_ID
#define MAKE_ID(a,b,c,d) ((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))
#endif

void Error(char *szError)
{
	printf("%s", szError);
}


/*************
 * FUNCTION:      code
 * DESCRIPTION:   create keyfile
 * INPUT:         version     AMIGA or PC
 *                name        name of registered user
 *                serial      serial number
 *                file        output filename
 *                type        type of file to create (old = 0, new = 1)
 * OUTPUT:        TRUE if ok else FALSE
 *************/
BOOL code(int version, char name[], int serial, char file[], ULONG type)
{
	int i,j;
	ULONG r,c;
	FILE *keyfile;
	USER user;

	if(!type)
	{
		// copy name to array
		strcpy((char *)&data[4],name);

		data[29] = serial >> 24;
		data[30] = (serial >> 16) & 0xff;
		data[31] = (serial >> 8) & 0xff;
		data[32] = serial & 0xff;

		if(version == AMIGA)
			data[3] = 'A';
		else
			data[3] = 'P';
	}
	else
	{
		strcpy(user.name, name);
		if(strlen(name) < 40)
		{
			// confuse this damned hackers a little bit
			// by inseting random numbers
			for(i=strlen(name)+1; i<40; i++)
				user.name[i] = rand();
		}

#ifdef __AMIGA__
		user.sernum = serial;
#else
		user.sernum = Swap(serial);
#endif
		if(version == AMIGA)
			user.flags = FLAGS_SYSTEM_AMIGA;
		else
			user.flags = FLAGS_SYSTEM_PC;
	}

	keyfile = fopen(file,"wb");
	if(!keyfile)
	{
		Error("Write error");
		return FALSE;
	}

	if(!type)
	{
		if(fwrite(&data,1,33,keyfile) != 33)
		{
			Error("Write error");
			fclose(keyfile);
			return FALSE;
		}
	}
	else
	{
		fprintf(keyfile, "// this file was automatically generated\n");
		fprintf(keyfile, "// do not edit or change it\n");
		fprintf(keyfile, "USER user =\n");
		fprintf(keyfile, "{\n");
		fprintf(keyfile, "	{ ");
		for(i=0; i<40; i++)
			fprintf(keyfile, "%d, ", user.name[i]);
		fprintf(keyfile, "},\n");
		fprintf(keyfile, "   %d,\n", serial);
		fprintf(keyfile, "   %s\n", version == AMIGA ? "FLAGS_SYSTEM_AMIGA" : "FLAGS_SYSTEM_PC");
		fprintf(keyfile, "};\n\n");
		fprintf(keyfile, "ULONG coded_user[40] =\n");
		fprintf(keyfile, "{\n");
	}

	// set random seed
	r = serial;

	for(i=0; i<(type ? 40 : 33); i++)
	{
		if(!type)
			c = data[i];
		else
			c = user.name[i];
		for(j=0; j<i+1; j++)
		{
			r = (240995 + 4194301*r) % 2147483647;
			c += r;
		}
		if(!type)
		{
			if(fwrite(&c,1,4,keyfile) != 4)
			{
				Error("Write error");
				fclose(keyfile);
				return FALSE;
			}
		}
		else
		{
			if(fprintf(keyfile, "   0x%08x,\n", c) < 0)
			{
				Error("Write error");
				fclose(keyfile);
				return FALSE;
			}
		}
	}

	if(type)
		fprintf(keyfile, "};\n");

	fclose(keyfile);

	return TRUE;
}

void main(int argc, char *argv[])
{
	int nComputer, nSernumber;

	if (!stricmp(argv[1], "PC"))
		nComputer = PC;
	else
		nComputer = AMIGA;

	nSernumber = atoi(argv[3]);

	code(nComputer, argv[2], nSernumber, "codekey.h", 1);
}
