/* PPC stubs */

#include <libraries/powerpc.h>
#include <pragma/powerpc_lib.h>

extern "ASM" void _PPCStub_Load__PcP05IMAGE(void);
extern "ASM" short Load__PcP05IMAGE(void *arg1,void *arg2)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_Load__PcP05IMAGE)(void) = _PPCStub_Load__PcP05IMAGE;
	ppc.PP_Code = _PPCData_Load__PcP05IMAGE;
	long args[2];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (short) ppc.PP_Regs[0];
}

extern "ASM" void _PPCStub_Cleanup__P05IMAGE(void);
extern "ASM" void Cleanup__P05IMAGE(void *arg1)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_Cleanup__P05IMAGE)(void) = _PPCStub_Cleanup__P05IMAGE;
	ppc.PP_Code = _PPCData_Cleanup__P05IMAGE;
	long args[1];
	args[0] = (long) arg1;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
}

