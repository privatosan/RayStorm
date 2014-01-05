
#ifdef __AMIGA__

#include <pragma/exec_lib.h>
#include <memory.h>
#include "keyfile_lib.h"
#include "rsi.h"

#ifndef __MIXEDBINARY__
#include <exec/libraries.h>

struct RayStormBase
{
	struct Library base;
};

#pragma libbase RayStormBase

struct Library *KeyfileBase=NULL;

extern "C" void INIT_3_init(void)
{
	KeyfileBase = OpenLibrary("S:RayStorm.key", 1L);
}

extern "C" void EXIT_3_exit(void)
{
	if(KeyfileBase)
		CloseLibrary(KeyfileBase);
}

#else // !__MIXEDBINARY__

#include <clib/alib_protos.h>

#ifndef TEXTURE_H
#include "texture.h"
#endif

extern "ASM" void abort__STANDARD() {}

extern "ASM" short __saveds _68kStub_stub_keyDecodeKeyfile1_(register __a0 long *args)
{
	return keyDecodeKeyfile1();
}
extern "ASM" short __saveds _68kStub_stub_keyDecodeKeyfile2_(register __a0 long *args)
{
	return keyDecodeKeyfile2();
}
extern "ASM" short __saveds _68kStub_stub_keyWritePicture__PcP07PICTUREPc(register __a0 long *args)
{
	return keyWritePicture(
		(char*) args[0],
		(PICTURE*) args[1],
		(char*) args[2]
	);
}

extern "ASM" void __saveds _68kStub_ppc_texture_work__PFvPfP08IM_PATCHP06VECTORPfpPfP08IM_PATCHP06VECTORPf(register __a0 long *args)
{
	void (*work)(float*, IM_PATCH*, VECTOR*, float*) = (void (*)(float*, IM_PATCH*, VECTOR*, float*))args[0];

	work(
		(float*) args[1],
		(IM_PATCH*) args[2],
		(VECTOR*) args[3],
		(float*) args[4]
	);
}

extern "ASM" void *__saveds _68kStub_ppc_texture_init__PFP09IM_TTABLEjpj(register __a0 long *args)
{
	IM_TTABLE* (*texture_init)(LONG) = (IM_TTABLE* (*)(LONG))args[0];

	return texture_init(
		(LONG) args[1]
	);
}

extern "ASM" void __saveds _68kStub_ppc_texture_cleanup__PFvp(register __a0 long *args)
{
	void (*cleanup)() = (void (*)())args[0];

	cleanup();
}

extern "ASM" unsigned long __saveds _68kStub_ppc_DoMethodA__PUjP00(register __a0 long *args)
{
	return DoMethodA((Object*)args[0], (Msg)args[1]);
}

#endif // __MIXEDBINARY__

rsiResult rsiGetLibInfo(rsiLIBINFO *info)
{
#ifdef __MIXEDBINARY__
	info->system = rsiSYSTEM_PPC_WarpOS;
#else
	info->system = rsiSYSTEM_M68K;
#endif // !__MIXEDBINARY__
	return rsiERR_NONE;
}

#else
	#include <windows.h>
	#include "lib.h"

char WorkingDirectory[256];

//
//  FUNCTION: DLLMain(HINSTANCE, DWORD, LPVOID)
//
//  PURPOSE:  Called when DLL is loaded by a process, and when new
//    threads are created by a process that has already loaded the
//    DLL.  Also called when threads of a process that has loaded the
//    DLL exit cleanly and when the process itself unloads the DLL.
//
//  PARAMETERS:
//    hDLLInst    - Instance handle of the DLL
//    fdwReason   - Process attach/detach or thread attach/detach
//    lpvReserved - Reserved and not used
//
//  RETURN VALUE:  (Used only when fdwReason == DLL_PROCESS_ATTACH)
//    TRUE  -  Used to signify that the DLL should remain loaded.
//    FALSE -  Used to signify that the DLL should be immediately unloaded.
//
//  COMMENTS:
//
//    If you want to use C runtime libraries, keep this function named
//    "DllMain" and you won't have to do anything special to initialize
//    the runtime libraries.
//
//    When fdwReason == DLL_PROCESS_ATTACH, the return value is used to
//    determine if the DLL should remain loaded, or should be immediately
//    unloaded depending upon whether the DLL could be initialized properly.
//    For all other values of fdwReason, the return value is ignored.
//

CRITICAL_SECTION gCriticalSection;

BOOL WINAPI DllMain(HINSTANCE hDLLInst, DWORD fdwReason, LPVOID lpvReserved)
{
	 switch (fdwReason)
	 {
	case DLL_PROCESS_ATTACH:
		 // The DLL is being loaded for the first time by a given process.
		 // Perform per-process initialization here.  If the initialization
		 // is successful, return TRUE; if unsuccessful, return FALSE.


		 // In this DLL, we want to initialize a critical section used
		 // by one of its exported functions, DLLFunction2().

		 InitializeCriticalSection(&gCriticalSection);
				WorkingDirectory[0] = '\0';
		 break;

	case DLL_PROCESS_DETACH:
		 // The DLL is being unloaded by a given process.  Do any
		 // per-process clean up here, such as undoing what was done in
		 // DLL_PROCESS_ATTACH.  The return value is ignored.


		 // In this DLL, we need to clean up the critical section we
		 // created in the DLL_PROCESS_ATTACH message.

		 DeleteCriticalSection(&gCriticalSection);

		 break;

	case DLL_THREAD_ATTACH:
		 // A thread is being created in a process that has already loaded
		 // this DLL.  Perform any per-thread initialization here.  The
		 // return value is ignored.

		 break;

	case DLL_THREAD_DETACH:
		 // A thread is exiting cleanly in a process that has already
		 // loaded this DLL.  Perform any per-thread clean up here.  The
		 // return value is ignored.

		 break;
	 }
	 return TRUE;
}
#endif
