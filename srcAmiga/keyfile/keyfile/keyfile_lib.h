/***************
 * NAME:          keyfile_lib.h
 * DESCRIPTION:   Definitions for the keyfile library
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    05.07.97 ah    initial release
 ***************/

#ifndef KEYFILE_LIB_H
#define KEYFILE_LIB_H

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef PICLOAD_H
#include "picload.h"
#endif

extern "AMIGA" KeyfileBase
{
	void keyGetInfo(char*, ULONG*) = -0x1e;
	BOOL keyDecodeKeyfile1() = -0x24;
	BOOL keyDecodeKeyfile2() = -0x2a;
	BOOL keyWritePicture(char*, PICTURE*, char *) = -0x30;
}

#endif
