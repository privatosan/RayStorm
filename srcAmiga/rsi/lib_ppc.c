#include <libraries/powerpc.h>
#include <pragma/powerpc_lib.h>

// stub functions

extern struct Library *PowerPCBase;
extern "ASM" void _68kStub_stub_keyDecodeKeyfile1_(void);
extern "ASM" short stub_keyDecodeKeyfile1_()
{
	struct PPCArgs ppc = { };
	static void (*_68kData_stub_keyDecodeKeyfile1_)(void) = _68kStub_stub_keyDecodeKeyfile1_;
	ppc.PP_Code = _68kData_stub_keyDecodeKeyfile1_;
	Run68K(&ppc);
	return (short) ppc.PP_Regs[0];
}

extern "ASM" void _68kStub_stub_keyDecodeKeyfile2_(void);
extern "ASM" short stub_keyDecodeKeyfile2_()
{
	struct PPCArgs ppc = { };
	static void (*_68kData_stub_keyDecodeKeyfile2_)(void) = _68kStub_stub_keyDecodeKeyfile2_;
	ppc.PP_Code = _68kData_stub_keyDecodeKeyfile2_;
	Run68K(&ppc);
	return (short) ppc.PP_Regs[0];
}

extern "ASM" void _68kStub_stub_keyWritePicture__PcP07PICTUREPc(void);
extern "ASM" short stub_keyWritePicture__PcP07PICTUREPc(void *arg1,void *arg2,void *arg3)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_stub_keyWritePicture__PcP07PICTUREPc)(void) = _68kStub_stub_keyWritePicture__PcP07PICTUREPc;
	ppc.PP_Code = _68kData_stub_keyWritePicture__PcP07PICTUREPc;
	long args[3];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) arg3;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
	return (short) ppc.PP_Regs[0];
}

extern "ASM" void _68kStub_ppc_texture_work__PFvPfP08IM_PATCHP06VECTORPfpPfP08IM_PATCHP06VECTORPf(void);
extern "ASM" void ppc_texture_work__PFvPfP08IM_PATCHP06VECTORPfpPfP08IM_PATCHP06VECTORPf(void *arg1,void *arg2,void *arg3,void *arg4,void *arg5)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_ppc_texture_work__PFvPfP08IM_PATCHP06VECTORPfpPfP08IM_PATCHP06VECTORPf)(void) = _68kStub_ppc_texture_work__PFvPfP08IM_PATCHP06VECTORPfpPfP08IM_PATCHP06VECTORPf;
	ppc.PP_Code = _68kData_ppc_texture_work__PFvPfP08IM_PATCHP06VECTORPfpPfP08IM_PATCHP06VECTORPf;
	long args[5];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) arg3;
	args[3] = (long) arg4;
	args[4] = (long) arg5;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
}

extern "ASM" void _68kStub_ppc_texture_init__PFP09IM_TTABLEjpj(void);
extern "ASM" void *ppc_texture_init__PFP09IM_TTABLEjpj(void *arg1,long arg2)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_ppc_texture_init__PFP09IM_TTABLEjpj)(void) = _68kStub_ppc_texture_init__PFP09IM_TTABLEjpj;
	ppc.PP_Code = _68kData_ppc_texture_init__PFP09IM_TTABLEjpj;
	long args[2];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
	return (void *) ppc.PP_Regs[0];
}

extern "ASM" void _68kStub_ppc_texture_cleanup__PFvp(void);
extern "ASM" void ppc_texture_cleanup__PFvp(void *arg1)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_ppc_texture_cleanup__PFvp)(void) = _68kStub_ppc_texture_cleanup__PFvp;
	ppc.PP_Code = _68kData_ppc_texture_cleanup__PFvp;
	long args[1];
	args[0] = (long) arg1;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
}

extern "ASM" void _68kStub_ppc_DoMethodA__PUjP00(void);
extern "ASM" unsigned long ppc_DoMethodA__PUjP00(void *arg1,void *arg2)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_ppc_DoMethodA__PUjP00)(void) = _68kStub_ppc_DoMethodA__PUjP00;
	ppc.PP_Code = _68kData_ppc_DoMethodA__PUjP00;
	long args[2];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
	return (unsigned long) ppc.PP_Regs[0];
}

// init functions

#include <pragma/exec_lib.h>
#include <exec/libraries.h>
#include <memory.h>

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

