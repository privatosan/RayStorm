/***************
 * MODUL:         typedefs
 * NAME:          typedefs.h
 * DESCRIPTION:   this file includes all typedefinitions
 *	AUTHORS:			Andreas Heumann, Mike Hesser
 * HISTORY:
 *		DATE		NAME	COMMENT
 *		11.02.95	ah		first release
 *		26.03.95	ah		changed 'typedef unsigned UWORD' to
 *			'typedef unsigned short UWORD'
 *		03.04.95	ah		now we are using the defines of the
 *			operating system
 *		20.09.95	ah		now one version for all platforms
 *		05.12.96	ah		added statistics
 *		09.07.97 ah		BOOL on non-AMIGA platforms were int (4 bytes)
 *			now it is unsigned short (2 bytes) (should save us LOTS of
 *			memory)
 ***************/

#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#ifdef __AMIGA__
	#ifndef	EXEC_TYPES_H
		#include <exec/types.h>
	#endif
	#ifndef STATS_H
	#include "stats.h"
	#endif
#else
	#define __SWAP__
	#if defined(__MFC__) && defined(__cplusplus)
		#include <afxwin.h>         // MFC core and standard components
		#include <afxext.h>         // MFC extensions (including VB)
		#include <afxtempl.h>
	#else
		#ifndef __WATCOMC__
			#include <windows.h>
		#endif
	#endif
	#ifdef __unix__
		#define stricmp strcasecmp
	#endif
	#define TRUE 1
	#define FALSE 0
	#ifndef NULL
		#define NULL 0
	#endif
	#ifdef __WATCOMC__
		#define abs(a) (a < 0 ? -a : a)
	#endif
	typedef unsigned char  BYTE;
	typedef long			  LONG;
	typedef unsigned long  ULONG;
	typedef unsigned short UWORD;
	typedef unsigned char  UBYTE;
	typedef void *APTR;
	typedef void *BPTR;

	#define STATISTIC(a)
	#define NULLSIZE ((SIZE)0L)	// no size
#endif

#define Swap(a) \
	(((ULONG)(a)&0xFF) << 24) | (((ULONG)(a)&0xFF00) << 8) | \
	(((ULONG)(a)&0xFF0000) >> 8) | (((ULONG)(a)&0xFF000000) >> 24)
#define SwapW(a) \
	(((UWORD)(a)&0xFF) << 8) | (((UWORD)(a)&0xFF00) >> 8)

#endif
