/***************
 * NAME:          objhand_lib.h
 * DESCRIPTION:   Definitions for the object handler libraries
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    09.02.97 ah    initial release
 *    15.05.97 ah    added version
 *    08.06.97 ah    the function back call table is moved from objInit() to objRead()
 ***************/

#ifndef OBJHAND_LIB_H
#define OBJHAND_LIB_H

#ifndef OBJLINK_H
#include "objlink.h"
#endif

#ifdef __PPC__
#if __STORM__ == 39
extern "library=ObjHandBase"
{
	BOOL objInit();
//   char *objRead_(struct Library*, rsiCONTEXT*, char*, OBJLINK*, VECTOR*, VECTOR*, VECTOR*, VECTOR*, VECTOR*, ACTOR*, SURFACE*, ULONG*);
	char *objRead(rsiCONTEXT*, char*, OBJLINK*, VECTOR*, VECTOR*, VECTOR*, VECTOR*, VECTOR*, ACTOR*, SURFACE*, ULONG*);
	void objCleanup();
}
#else
extern "AmigaLib" ObjHandBase
{
	BOOL objInit() = -0x1e;
//   char *objRead_(struct Library*, rsiCONTEXT*, char*, OBJLINK*, VECTOR*, VECTOR*, VECTOR*, VECTOR*, VECTOR*, ACTOR*, SURFACE*, ULONG*) = -0x24;
	char *objRead(rsiCONTEXT*, char*, OBJLINK*, VECTOR*, VECTOR*, VECTOR*, VECTOR*, VECTOR*, ACTOR*, SURFACE*, ULONG*) = -0x24;
	void objCleanup() = -0x2a;
}
#endif

/*char *objRead(rsiCONTEXT*a, char*b, OBJLINK*c, VECTOR*d, VECTOR*e, VECTOR*f, VECTOR*g, VECTOR*h, ACTOR*i, SURFACE*j, ULONG*k)
{
	extern struct Library *ObjHandBase;
	return objRead_(ObjHandBase, a,b,c,d,e,f,g,h,i,j,k);
}*/

#else
extern "AMIGA" ObjHandBase
{
	BOOL objInit() = -0x1e;
	char *objRead(rsiCONTEXT*, char*, OBJLINK*, VECTOR*, VECTOR*, VECTOR*, VECTOR*, VECTOR*, ACTOR*, SURFACE*, ULONG*) = -0x24;
	void objCleanup() = -0x2a;
}
#endif

#endif
