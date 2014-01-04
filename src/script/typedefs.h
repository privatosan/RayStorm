/***************
 * MODUL:         typedefs
 * NAME:          typedefs.h
 * VERSION:       1.1 20.09.1995
 * DESCRIPTION:   this file includes all typedefinitions
 *	AUTHORS:			Andreas Heumann, Mike Hesser
 * BUGS:          none
 * TO DO:         all
 * HISTORY:
 *		DATE		NAME	COMMENT
 *		11.02.95	ah		first release
 *		26.03.95	ah		changed 'typedef unsigned UWORD' to
 *			'typedef unsigned short UWORD' (0.11)
 *		03.04.95	ah		now we are using the defines of the
 *			operating system (0.12)
 *		04.05.95	ah		bumped version to V1 (1.0)
 *		20.09.95	ah		now one version for all platforms (1.1)
 ***************/

#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#ifdef __AMIGA__
	#ifndef	EXEC_TYPES_H
		#include <exec/types.h>
	#endif
#else
	#define __SWAP__
	#ifdef __unix__
		#define stricmp strcasecmp
	#endif
	#if defined __DOS__ || __unix__
		#define TRUE 1
		#define FALSE 0
		#ifndef NULL
			#define NULL 0
		#endif
	#endif
	#ifdef __cplusplus
		#include "stdafx.h"
	#else
		typedef int        BOOL;
		typedef long       LONG;
		typedef char       BYTE;
		typedef short      WORD;
	#endif
	typedef unsigned long  ULONG;
	typedef unsigned short UWORD;
	typedef unsigned char  UBYTE;
	#ifndef __NT__
//		typedef unsigned int   USHORT;
	#endif
	typedef void 		   *APTR;
	typedef void 		   *BPTR;
	
	#define NULLSIZE ((SIZE)0L)	// no size
#endif

#define Swap(a) \
	(((ULONG)(a)&0xFF) << 24) | (((ULONG)(a)&0xFF00) << 8) | \
	(((ULONG)(a)&0xFF0000) >> 8) | (((ULONG)(a)&0xFF000000) >> 24)
#define SwapW(a) \
	(((UWORD)(a)&0xFF) << 8) | (((UWORD)(a)&0xFF00) >> 8)

#endif
