/***************
 * MODUL:         Picture handler library definition file
 * NAME:          piclib.cpp
 * DESCRIPTION:   definitions for library
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    09.02.97 ah    initial releas
 *    11.02.97 ah    added ppc stub function
 ***************/

#include <memory.h>

#include "piclib.h"
#include "picload.h"
#include <libraries/powerpc.h>
#include <pragma/powerpc_lib.h>

#ifndef __MIXEDBINARY__
#pragma libbase PicHandBase
#else
extern char *ppc_picWrite(PICTURE*);

extern "ASM" void _PPCStub__ppc_picWrite(void);
extern "ASM" void *_ppc_picWrite(void *arg1)
{
	struct PPCArgs ppc = { };
	long args[1];
	static void (*_PPCData__ppc_picWrite)(void) = _PPCStub__ppc_picWrite;
	ppc.PP_Code = _PPCData__ppc_picWrite;
	args[0] = (long) arg1;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (void *) ppc.PP_Regs[0];
}

char *picWrite(PICTURE *image)
{
	return ppc_picWrite(image);
}
#endif // __MIXEDBINARY__
