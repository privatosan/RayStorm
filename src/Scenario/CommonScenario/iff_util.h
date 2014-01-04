/***************
 * PROGRAM:       Modeler
 * NAME:          iff_util.h
 * VERSION:       1.0 31.05.1996
 * DESCRIPTION:   definitions for iff util routines
 *	AUTHORS:			Andreas Heumann
 * HISTORY:
 *		DATE		NAME	COMMENT
 *		31.05.96	ah		initial release
 *		02.06.96 mh		WriteLongChunk
 ***************/

#ifndef IFF_UTIL_H
#define IFF_UTIL_H

#ifdef __AMIGA__
	#ifdef __SASC
		#include <proto/iffparse.h>
	#endif
	#if defined __MAXON__ || __STORM__
		#include <pragma/iffparse_lib.h>
		#include <clib/iffparse_protos.h>
	#endif

	#ifndef IFF_IFFPARSE_H
	#include <libraries/iffparse.h>
	#endif
#else
	#include "iffparse.h"
#endif

// macros
#define PUSH_CHUNK(id) \
if(PushChunk(iff, id, ID_FORM, IFFSIZE_UNKNOWN)) \
{ \
	IFFCleanup(iff); \
	return errors[ERR_IFFPARSE]; \
}

#define WRITE_CHUNK(id,data,size) \
if(!WriteChunk(iff, id, data, size)) \
{ \
	IFFCleanup(iff); \
	return errors[ERR_WRITE]; \
}

#define WRITE_LONG_CHUNK(id,data,cnt) \
if(!WriteLongChunk(iff, id, data, cnt)) \
{ \
	IFFCleanup(iff); \
	return errors[ERR_WRITE]; \
}

#define POP_CHUNK() \
if(PopChunk(iff)) \
{ \
	IFFCleanup(iff); \
	return errors[ERR_IFFPARSE]; \
}

BOOL WriteChunk(struct IFFHandle *, LONG, APTR, LONG);
BOOL WriteLongChunk(struct IFFHandle *, LONG, APTR, int);
BOOL WriteWordChunk(struct IFFHandle *, LONG, APTR, int);
BOOL ReadInt(IFFHandle*, int*, int);
BOOL ReadULONG(IFFHandle*, ULONG*, int);
BOOL ReadWord(IFFHandle*, WORD*, int);
int ReadString(IFFHandle*, char*, int);
BOOL ReadFloat(IFFHandle*, float*, int);
BOOL ReadUBYTE(IFFHandle*, UBYTE*, int);
void IFFCleanup(struct IFFHandle *iff);

#endif
