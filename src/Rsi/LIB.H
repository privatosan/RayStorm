#ifndef LIB_H
#define LIB_H

#ifdef __AMIGA__

extern struct Library *KeyfileBase;
#ifdef __PPC__

#include <clib/alib_protos.h>

#ifndef TEXTURE_H
#include "texture.h"
#endif

#include "picload.h"

void stub_keyGetInfo(char*, ULONG*);
BOOL stub_keyDecodeKeyfile1();
BOOL stub_keyDecodeKeyfile2();
BOOL stub_keyWritePicture(char*, PICTURE*, char *);

#define keyGetInfo(a,b) stub_keyGetInfo(a,b)
#define keyDecodeKeyfile1() stub_keyDecodeKeyfile1()
#define keyDecodeKeyfile2() stub_keyDecodeKeyfile2()
#define keyWritePicture(a,b,c) stub_keyWritePicture(a,b,c)

IM_TTABLE *ppc_texture_init(IM_TTABLE* (*texture_init)(LONG), LONG);
void ppc_texture_work(void (*work)(float*, IM_PATCH*, VECTOR*, float*), float*, IM_PATCH*, VECTOR*, float*);
void ppc_texture_cleanup(void (*cleanup)());
ULONG ppc_DoMethodA(Object*, Msg);

#else
#include "keyfile_lib.h"
#endif // __PPC__

#else
	// Constant definitions

	#define DLLEXPORT __declspec(dllexport)

	// Global variables

	extern CRITICAL_SECTION gCriticalSection;
	extern float            gRoot;

	// Product identifier string defines

	#define SZAPPNAME     "RayStorm renderer DLL"
	#define SZDESCRIPTION "Win32 renderer DLL"
	#define SZVERSION     "Version 1.25"

	// Version string definitions

	#define SZRCOMPANYNAME "RayStorm"
	#define SZRDESCRIPTION "RayStorm renderer DLL"
	#define SZRVERSION     "v1.25"
	#define SZRAPPNAME     "RSI"
	#define SZRCOPYRIGHT   "Copyright (c) 1995/96 Andreas Heumann & Mike Hesser"
	#define SZRTRADEMARK   "RayStorm (tm)"
	#define SZRPRODNAME    "RSI.DLL"
	#define SZRPRODVER     "RSI v1.25"
#endif
#endif
