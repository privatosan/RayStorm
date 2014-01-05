#include <clib/alib_protos.h>
#include <libraries/powerpc.h>
#include <pragma/powerpc_lib.h>
#include "muiutility.h"

#include "typedefs.h"

//------------------------------------------------
// amiga lib stubs

extern "ASM" unsigned long __saveds _68kStub_DoMethodAPPC__PUjP00(register __a0 long *args)
{
	return DoMethodA(
		(Object*)args[0],
		(Msg) args[1]);
}

extern "ASM" unsigned long __saveds _68kStub_DoSuperMethodAPPC__P06IClassPUjP00(register __a0 long *args)
{
	return DoSuperMethodA(
		(struct IClass*) args[0],
		(Object*) args[1],
		(Msg) args[2]);
}

extern "ASM" void *__saveds _68kStub_MUI_NewObjectAPPC__PcP07TagItem(register __a0 long *args)
{
	return MUI_NewObjectA(
		(char*) args[0],
		(struct TagItem*) args[1]
	);
}

extern "ASM" void *MUI_MakeObjectAPPC__UjPUj(unsigned long arg1,void *arg2);
extern "ASM" void *__saveds _68kStub_MUI_MakeObjectAPPC__UjPUj(register __a0 long *args)
{
	return MUI_MakeObjectA(
		(ULONG) args[0],
		(ULONG*) args[1]);
}

//------------------------------------------------
// RayStorm Texture stubs

extern "ASM" void _PPCStub__PPCRTextureWork(void);
extern "ASM" void _PPCRTextureWork(void *arg1,void *arg2,void *arg3,void *arg4)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData__PPCRTextureWork)(void) = _PPCStub__PPCRTextureWork;
	ppc.PP_Code = _PPCData__PPCRTextureWork;
	long args[4];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) arg3;
	args[3] = (long) arg4;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
}

extern "ASM" void _PPCStub_PPCRTextureCleanup__PFvP06_tinfopP06_tinfoP07Library(void);
extern "ASM" void PPCRTextureCleanup__PFvP06_tinfopP06_tinfoP07Library(void *arg1,void *arg2,void *arg3)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_PPCRTextureCleanup__PFvP06_tinfopP06_tinfoP07Library)(void) = _PPCStub_PPCRTextureCleanup__PFvP06_tinfopP06_tinfoP07Library;
	ppc.PP_Code = _PPCData_PPCRTextureCleanup__PFvP06_tinfopP06_tinfoP07Library;
	long args[3];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) arg3;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
}

extern "ASM" void _PPCStub_stub_texture_init__PcPP07Library(void);
extern "ASM" void *stub_texture_init__PcPP07Library(void *arg1,void *arg2)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_texture_init__PcPP07Library)(void) = _PPCStub_stub_texture_init__PcPP07Library;
	ppc.PP_Code = _PPCData_stub_texture_init__PcPP07Library;
	long args[2];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (void *) ppc.PP_Regs[0];
}

//------------------------------------------------
// Preview Texture stubs

#include "preview.h"

extern "ASM" void __saveds _68kStub_ITextureWork__P09IM_TTABLEPfP08IM_PATCHP06VECTORPf(register __a0 long *args)
{
	((IM_TTABLE*)args[0])->work(
		(float *) args[1],
		(IM_PATCH *) args[2],
		(VECTOR *) args[3],
		(float *) args[4]
	);
}

extern "ASM" void *__saveds _68kStub_ITextureInit__PFP09IM_TTABLEjp(register __a0 long *args)
{
	IM_TTABLE* (*texture_init)(LONG arg0) = (IM_TTABLE* (*)(LONG))args[0];

	return texture_init(0x60FFFF);
}

extern "ASM" void __saveds _68kStub_ITextureCleanup__P09IM_TTABLE(register __a0 long *args)
{
	((IM_TTABLE*)args[0])->cleanup();
}

extern "ASM" void *__saveds _68kStub_GetNext__P07TEXTURE(register __a0 long *args)
{
	return ((TEXTURE*)args[0])->GetNext();
}

extern "ASM" void *__saveds _68kStub_GetNext__P14TEXTURE_OBJECT(register __a0 long *args)
{
	return ((TEXTURE_OBJECT*)args[0])->GetNext();
}

extern "ASM" long __saveds _68kStub_GetType__P07TEXTURE(register __a0 long *args)
{
	return ((TEXTURE*)args[0])->GetType();
}

extern "ASM" void _PPCStub_PreviewCreate_(void);
extern "ASM" void *PreviewCreate_()
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_PreviewCreate_)(void) = _PPCStub_PreviewCreate_;
	ppc.PP_Code = _PPCData_PreviewCreate_;
	RunPPC(&ppc);
	return (void *) ppc.PP_Regs[0];
}

extern "ASM" void _PPCStub_Init__P07PREVIEWiiiP07SURFACEP06OBJECT(void);
extern "ASM" short Init__P07PREVIEWiiiP07SURFACEP06OBJECT(void *arg1,long arg2,long arg3,long arg4,void *arg5,void *arg6)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_Init__P07PREVIEWiiiP07SURFACEP06OBJECT)(void) = _PPCStub_Init__P07PREVIEWiiiP07SURFACEP06OBJECT;
	ppc.PP_Code = _PPCData_Init__P07PREVIEWiiiP07SURFACEP06OBJECT;
	long args[6];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) arg3;
	args[3] = (long) arg4;
	args[4] = (long) arg5;
	args[5] = (long) arg6;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (short) ppc.PP_Regs[0];
}

extern "ASM" void _PPCStub_RenderLines__P07PREVIEWi(void);
extern "ASM" void RenderLines__P07PREVIEWi(void *arg1,long arg2)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_RenderLines__P07PREVIEWi)(void) = _PPCStub_RenderLines__P07PREVIEWi;
	ppc.PP_Code = _PPCData_RenderLines__P07PREVIEWi;
	long args[2];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
}

extern "ASM" void _PPCStub_GetLine__P07PREVIEW(void);
extern "ASM" void *GetLine__P07PREVIEW(void *arg1)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_GetLine__P07PREVIEW)(void) = _PPCStub_GetLine__P07PREVIEW;
	ppc.PP_Code = _PPCData_GetLine__P07PREVIEW;
	long args[1];
	args[0] = (long) arg1;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (void *) ppc.PP_Regs[0];
}

extern "ASM" void _PPCStub_PreviewDelete__P07PREVIEW(void);
extern "ASM" void PreviewDelete__P07PREVIEW(void *arg1)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_PreviewDelete__P07PREVIEW)(void) = _PPCStub_PreviewDelete__P07PREVIEW;
	ppc.PP_Code = _PPCData_PreviewDelete__P07PREVIEW;
	long args[1];
	args[0] = (long) arg1;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
}

//------------------------------------------------
// rsi stubs

extern "ASM" void _PPCStub_ppcRayStormLibOpen_(void);
extern "ASM" short ppcRayStormLibOpen_()
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_ppcRayStormLibOpen_)(void) = _PPCStub_ppcRayStormLibOpen_;
	ppc.PP_Code = _PPCData_ppcRayStormLibOpen_;
	RunPPC(&ppc);
	return (short) ppc.PP_Regs[0];
}
extern "ASM" void _PPCStub_stub_rsiSavePicture__PvPce(void);
extern "ASM" long stub_rsiSavePicture__PvPce(void *arg1,void *arg2)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_rsiSavePicture__PvPce)(void) = _PPCStub_stub_rsiSavePicture__PvPce;
	ppc.PP_Code = _PPCData_stub_rsiSavePicture__PvPce;
	long args[3];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) ((&arg2)+1);
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (long) ppc.PP_Regs[0];
}
extern "ASM" void _PPCStub_stub_rsiCreateStar__PvPvPv(void);
extern "ASM" long stub_rsiCreateStar__PvPvPv(void *arg1,void *arg2,void *arg3)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_rsiCreateStar__PvPvPv)(void) = _PPCStub_stub_rsiCreateStar__PvPvPv;
	ppc.PP_Code = _PPCData_stub_rsiCreateStar__PvPvPv;
	long args[3];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) arg3;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (long) ppc.PP_Regs[0];
}
extern "ASM" void _PPCStub_stub_rsiSetSurface__PvPve(void);
extern "ASM" long stub_rsiSetSurface__PvPve(void *arg1,void *arg2)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_rsiSetSurface__PvPve)(void) = _PPCStub_stub_rsiSetSurface__PvPve;
	ppc.PP_Code = _PPCData_stub_rsiSetSurface__PvPve;
	long args[3];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) ((&arg2)+1);
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (long) ppc.PP_Regs[0];
}
extern "ASM" void _PPCStub_stub_rsiSetTexture__PvPve(void);
extern "ASM" long stub_rsiSetTexture__PvPve(void *arg1,void *arg2)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_rsiSetTexture__PvPve)(void) = _PPCStub_stub_rsiSetTexture__PvPve;
	ppc.PP_Code = _PPCData_stub_rsiSetTexture__PvPve;
	long args[3];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) ((&arg2)+1);
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (long) ppc.PP_Regs[0];
}
extern "ASM" void _PPCStub_stub_rsiSetBrush__PvPve(void);
extern "ASM" long stub_rsiSetBrush__PvPve(void *arg1,void *arg2)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_rsiSetBrush__PvPve)(void) = _PPCStub_stub_rsiSetBrush__PvPve;
	ppc.PP_Code = _PPCData_stub_rsiSetBrush__PvPve;
	long args[3];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) ((&arg2)+1);
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (long) ppc.PP_Regs[0];
}
extern "ASM" void _PPCStub_stub_rsiSetCone__PvPve(void);
extern "ASM" long stub_rsiSetCone__PvPve(void *arg1,void *arg2)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_rsiSetCone__PvPve)(void) = _PPCStub_stub_rsiSetCone__PvPve;
	ppc.PP_Code = _PPCData_stub_rsiSetCone__PvPve;
	long args[3];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) ((&arg2)+1);
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (long) ppc.PP_Regs[0];
}
extern "ASM" void _PPCStub_stub_rsiSetFlare__PvPve(void);
extern "ASM" long stub_rsiSetFlare__PvPve(void *arg1,void *arg2)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_rsiSetFlare__PvPve)(void) = _PPCStub_stub_rsiSetFlare__PvPve;
	ppc.PP_Code = _PPCData_stub_rsiSetFlare__PvPve;
	long args[3];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) ((&arg2)+1);
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (long) ppc.PP_Regs[0];
}
extern "ASM" void _PPCStub_stub_rsiSetSpotLight__PvPve(void);
extern "ASM" long stub_rsiSetSpotLight__PvPve(void *arg1,void *arg2)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_rsiSetSpotLight__PvPve)(void) = _PPCStub_stub_rsiSetSpotLight__PvPve;
	ppc.PP_Code = _PPCData_stub_rsiSetSpotLight__PvPve;
	long args[3];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) ((&arg2)+1);
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (long) ppc.PP_Regs[0];
}
extern "ASM" void _PPCStub_stub_rsiSetPlane__PvPve(void);
extern "ASM" long stub_rsiSetPlane__PvPve(void *arg1,void *arg2)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_rsiSetPlane__PvPve)(void) = _PPCStub_stub_rsiSetPlane__PvPve;
	ppc.PP_Code = _PPCData_stub_rsiSetPlane__PvPve;
	long args[3];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) ((&arg2)+1);
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (long) ppc.PP_Regs[0];
}
extern "ASM" void _PPCStub_stub_rsiSetSOR__PvPve(void);
extern "ASM" long stub_rsiSetSOR__PvPve(void *arg1,void *arg2)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_rsiSetSOR__PvPve)(void) = _PPCStub_stub_rsiSetSOR__PvPve;
	ppc.PP_Code = _PPCData_stub_rsiSetSOR__PvPve;
	long args[3];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) ((&arg2)+1);
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (long) ppc.PP_Regs[0];
}
extern "ASM" void _PPCStub_stub_rsiCreateSphere__PvPPvPv(void);
extern "ASM" long stub_rsiCreateSphere__PvPPvPv(void *arg1,void *arg2,void *arg3)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_rsiCreateSphere__PvPPvPv)(void) = _PPCStub_stub_rsiCreateSphere__PvPPvPv;
	ppc.PP_Code = _PPCData_stub_rsiCreateSphere__PvPPvPv;
	long args[3];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) arg3;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (long) ppc.PP_Regs[0];
}
extern "ASM" void _PPCStub_stub_rsiCreateBox__PvPPvPv(void);
extern "ASM" long stub_rsiCreateBox__PvPPvPv(void *arg1,void *arg2,void *arg3)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_rsiCreateBox__PvPPvPv)(void) = _PPCStub_stub_rsiCreateBox__PvPPvPv;
	ppc.PP_Code = _PPCData_stub_rsiCreateBox__PvPPvPv;
	long args[3];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) arg3;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (long) ppc.PP_Regs[0];
}
extern "ASM" void _PPCStub_stub_rsiSetCamera__Pve(void);
extern "ASM" long stub_rsiSetCamera__Pve(void *arg1)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_rsiSetCamera__Pve)(void) = _PPCStub_stub_rsiSetCamera__Pve;
	ppc.PP_Code = _PPCData_stub_rsiSetCamera__Pve;
	long args[2];
	args[0] = (long) arg1;
	args[1] = (long) ((&arg1)+1);
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (long) ppc.PP_Regs[0];
}
extern "ASM" void _PPCStub_stub_rsiSetCSG__PvPve(void);
extern "ASM" long stub_rsiSetCSG__PvPve(void *arg1,void *arg2)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_rsiSetCSG__PvPve)(void) = _PPCStub_stub_rsiSetCSG__PvPve;
	ppc.PP_Code = _PPCData_stub_rsiSetCSG__PvPve;
	long args[3];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) ((&arg2)+1);
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (long) ppc.PP_Regs[0];
}
extern "ASM" void _PPCStub_stub_rsiCreateCylinder__PvPPvPv(void);
extern "ASM" long stub_rsiCreateCylinder__PvPPvPv(void *arg1,void *arg2,void *arg3)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_rsiCreateCylinder__PvPPvPv)(void) = _PPCStub_stub_rsiCreateCylinder__PvPPvPv;
	ppc.PP_Code = _PPCData_stub_rsiCreateCylinder__PvPPvPv;
	long args[3];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) arg3;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (long) ppc.PP_Regs[0];
}
extern "ASM" void _PPCStub_ppcRayStormLibClose_(void);
extern "ASM" void ppcRayStormLibClose_()
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_ppcRayStormLibClose_)(void) = _PPCStub_ppcRayStormLibClose_;
	ppc.PP_Code = _PPCData_ppcRayStormLibClose_;
	RunPPC(&ppc);
}
extern "ASM" void _PPCStub_stub_rsiCleanup__Pv(void);
extern "ASM" long stub_rsiCleanup__Pv(void *arg1)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_rsiCleanup__Pv)(void) = _PPCStub_stub_rsiCleanup__Pv;
	ppc.PP_Code = _PPCData_stub_rsiCleanup__Pv;
	long args[1];
	args[0] = (long) arg1;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (long) ppc.PP_Regs[0];
}
extern "ASM" void _PPCStub_stub_rsiCreateSOR__PvPPvPviP11rsiVECTOR2D(void);
extern "ASM" long stub_rsiCreateSOR__PvPPvPviP11rsiVECTOR2D(void *arg1,void *arg2,void *arg3,long arg4,void *arg5)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_rsiCreateSOR__PvPPvPviP11rsiVECTOR2D)(void) = _PPCStub_stub_rsiCreateSOR__PvPPvPviP11rsiVECTOR2D;
	ppc.PP_Code = _PPCData_stub_rsiCreateSOR__PvPPvPviP11rsiVECTOR2D;
	long args[5];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) arg3;
	args[3] = (long) arg4;
	args[4] = (long) arg5;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (long) ppc.PP_Regs[0];
}
extern "ASM" void _PPCStub_stub_rsiSetSphere__PvPve(void);
extern "ASM" long stub_rsiSetSphere__PvPve(void *arg1,void *arg2)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_rsiSetSphere__PvPve)(void) = _PPCStub_stub_rsiSetSphere__PvPve;
	ppc.PP_Code = _PPCData_stub_rsiSetSphere__PvPve;
	long args[3];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) ((&arg2)+1);
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (long) ppc.PP_Regs[0];
}
extern "ASM" void _PPCStub_stub_rsiSetStar__PvPve(void);
extern "ASM" long stub_rsiSetStar__PvPve(void *arg1,void *arg2)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_rsiSetStar__PvPve)(void) = _PPCStub_stub_rsiSetStar__PvPve;
	ppc.PP_Code = _PPCData_stub_rsiSetStar__PvPve;
	long args[3];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) ((&arg2)+1);
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (long) ppc.PP_Regs[0];
}
extern "ASM" void _PPCStub_stub_rsiCreateSurface__PvPPve(void);
extern "ASM" long stub_rsiCreateSurface__PvPPve(void *arg1,void *arg2)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_rsiCreateSurface__PvPPve)(void) = _PPCStub_stub_rsiCreateSurface__PvPPve;
	ppc.PP_Code = _PPCData_stub_rsiCreateSurface__PvPPve;
	long args[3];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) ((&arg2)+1);
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (long) ppc.PP_Regs[0];
}
extern "ASM" void _PPCStub_stub_rsiAddImagineTexture__PvPPvPcPv(void);
extern "ASM" long stub_rsiAddImagineTexture__PvPPvPcPv(void *arg1,void *arg2,void *arg3,void *arg4)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_rsiAddImagineTexture__PvPPvPcPv)(void) = _PPCStub_stub_rsiAddImagineTexture__PvPPvPcPv;
	ppc.PP_Code = _PPCData_stub_rsiAddImagineTexture__PvPPvPcPv;
	long args[4];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) arg3;
	args[3] = (long) arg4;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (long) ppc.PP_Regs[0];
}
extern "ASM" void _PPCStub_stub_rsiAddRayStormTexture__PvPPvPcPv(void);
extern "ASM" long stub_rsiAddRayStormTexture__PvPPvPcPv(void *arg1,void *arg2,void *arg3,void *arg4)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_rsiAddRayStormTexture__PvPPvPcPv)(void) = _PPCStub_stub_rsiAddRayStormTexture__PvPPvPcPv;
	ppc.PP_Code = _PPCData_stub_rsiAddRayStormTexture__PvPPvPcPv;
	long args[4];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) arg3;
	args[3] = (long) arg4;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (long) ppc.PP_Regs[0];
}
extern "ASM" void _PPCStub_stub_rsiSetBox__PvPve(void);
extern "ASM" long stub_rsiSetBox__PvPve(void *arg1,void *arg2)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_rsiSetBox__PvPve)(void) = _PPCStub_stub_rsiSetBox__PvPve;
	ppc.PP_Code = _PPCData_stub_rsiSetBox__PvPve;
	long args[3];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) ((&arg2)+1);
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (long) ppc.PP_Regs[0];
}
extern "ASM" void _PPCStub_stub_rsiAddBrush__PvPPvPcPv(void);
extern "ASM" long stub_rsiAddBrush__PvPPvPcPv(void *arg1,void *arg2,void *arg3,void *arg4)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_rsiAddBrush__PvPPvPcPv)(void) = _PPCStub_stub_rsiAddBrush__PvPPvPcPv;
	ppc.PP_Code = _PPCData_stub_rsiAddBrush__PvPPvPcPv;
	long args[4];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) arg3;
	args[3] = (long) arg4;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (long) ppc.PP_Regs[0];
}
extern "ASM" void _PPCStub_stub_rsiCreateCone__PvPPvPv(void);
extern "ASM" long stub_rsiCreateCone__PvPPvPv(void *arg1,void *arg2,void *arg3)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_rsiCreateCone__PvPPvPv)(void) = _PPCStub_stub_rsiCreateCone__PvPPvPv;
	ppc.PP_Code = _PPCData_stub_rsiCreateCone__PvPPvPv;
	long args[3];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) arg3;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (long) ppc.PP_Regs[0];
}
extern "ASM" void _PPCStub_stub_rsiCreateCSG__PvPPv(void);
extern "ASM" long stub_rsiCreateCSG__PvPPv(void *arg1,void *arg2)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_rsiCreateCSG__PvPPv)(void) = _PPCStub_stub_rsiCreateCSG__PvPPv;
	ppc.PP_Code = _PPCData_stub_rsiCreateCSG__PvPPv;
	long args[2];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (long) ppc.PP_Regs[0];
}
extern "ASM" void _PPCStub_stub_rsiSetCylinder__PvPve(void);
extern "ASM" long stub_rsiSetCylinder__PvPve(void *arg1,void *arg2)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_rsiSetCylinder__PvPve)(void) = _PPCStub_stub_rsiSetCylinder__PvPve;
	ppc.PP_Code = _PPCData_stub_rsiSetCylinder__PvPve;
	long args[3];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) ((&arg2)+1);
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (long) ppc.PP_Regs[0];
}
extern "ASM" void _PPCStub_stub_rsiCreateFlare__PvPvPv(void);
extern "ASM" long stub_rsiCreateFlare__PvPvPv(void *arg1,void *arg2,void *arg3)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_rsiCreateFlare__PvPvPv)(void) = _PPCStub_stub_rsiCreateFlare__PvPvPv;
	ppc.PP_Code = _PPCData_stub_rsiCreateFlare__PvPvPv;
	long args[3];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) arg3;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (long) ppc.PP_Regs[0];
}
extern "ASM" void _PPCStub_stub_rsiCreatePointLight__PvPPv(void);
extern "ASM" long stub_rsiCreatePointLight__PvPPv(void *arg1,void *arg2)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_rsiCreatePointLight__PvPPv)(void) = _PPCStub_stub_rsiCreatePointLight__PvPPv;
	ppc.PP_Code = _PPCData_stub_rsiCreatePointLight__PvPPv;
	long args[2];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (long) ppc.PP_Regs[0];
}
extern "ASM" void _PPCStub_stub_rsiSetLogCB__PvPFvPvPcp(void);
extern "ASM" long stub_rsiSetLogCB__PvPFvPvPcp(void *arg1,void *arg2)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_rsiSetLogCB__PvPFvPvPcp)(void) = _PPCStub_stub_rsiSetLogCB__PvPFvPvPcp;
	ppc.PP_Code = _PPCData_stub_rsiSetLogCB__PvPFvPvPcp;
	long args[2];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (long) ppc.PP_Regs[0];
}
extern "ASM" void _PPCStub_stub_rsiGetErrorMsg__PvPci(void);
extern "ASM" long stub_rsiGetErrorMsg__PvPci(void *arg1,void *arg2,long arg3)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_rsiGetErrorMsg__PvPci)(void) = _PPCStub_stub_rsiGetErrorMsg__PvPci;
	ppc.PP_Code = _PPCData_stub_rsiGetErrorMsg__PvPci;
	long args[3];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) arg3;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (long) ppc.PP_Regs[0];
}
extern "ASM" void _PPCStub_stub_rsiSetPointLight__PvPve(void);
extern "ASM" long stub_rsiSetPointLight__PvPve(void *arg1,void *arg2)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_rsiSetPointLight__PvPve)(void) = _PPCStub_stub_rsiSetPointLight__PvPve;
	ppc.PP_Code = _PPCData_stub_rsiSetPointLight__PvPve;
	long args[3];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) ((&arg2)+1);
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (long) ppc.PP_Regs[0];
}
extern "ASM" void _PPCStub_stub_rsiCreatePlane__PvPPvPv(void);
extern "ASM" long stub_rsiCreatePlane__PvPPvPv(void *arg1,void *arg2,void *arg3)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_rsiCreatePlane__PvPPvPv)(void) = _PPCStub_stub_rsiCreatePlane__PvPPvPv;
	ppc.PP_Code = _PPCData_stub_rsiCreatePlane__PvPPvPv;
	long args[3];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) arg3;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (long) ppc.PP_Regs[0];
}
extern "ASM" void _PPCStub_stub_rsiSetCooperateCB__PvPFvPvp(void);
extern "ASM" long stub_rsiSetCooperateCB__PvPFvPvp(void *arg1,void *arg2)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_rsiSetCooperateCB__PvPFvPvp)(void) = _PPCStub_stub_rsiSetCooperateCB__PvPFvPvp;
	ppc.PP_Code = _PPCData_stub_rsiSetCooperateCB__PvPFvPvp;
	long args[2];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (long) ppc.PP_Regs[0];
}
extern "ASM" void _PPCStub_stub_rsiCreateSpotLight__PvPPv(void);
extern "ASM" long stub_rsiCreateSpotLight__PvPPv(void *arg1,void *arg2)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_rsiCreateSpotLight__PvPPv)(void) = _PPCStub_stub_rsiCreateSpotLight__PvPPv;
	ppc.PP_Code = _PPCData_stub_rsiCreateSpotLight__PvPPv;
	long args[2];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (long) ppc.PP_Regs[0];
}
extern "ASM" void _PPCStub_stub_rsiSetUpdateStatusCB__PvPFvPvffiiP14rsiSMALL_COLORp(void);
extern "ASM" long stub_rsiSetUpdateStatusCB__PvPFvPvffiiP14rsiSMALL_COLORp(void *arg1,void *arg2)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_rsiSetUpdateStatusCB__PvPFvPvffiiP14rsiSMALL_COLORp)(void) = _PPCStub_stub_rsiSetUpdateStatusCB__PvPFvPvffiiP14rsiSMALL_COLORp;
	ppc.PP_Code = _PPCData_stub_rsiSetUpdateStatusCB__PvPFvPvffiiP14rsiSMALL_COLORp;
	long args[2];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (long) ppc.PP_Regs[0];
}
extern "ASM" void _PPCStub_stub_rsiSetUserData__PvPv(void);
extern "ASM" long stub_rsiSetUserData__PvPv(void *arg1,void *arg2)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_rsiSetUserData__PvPv)(void) = _PPCStub_stub_rsiSetUserData__PvPv;
	ppc.PP_Code = _PPCData_stub_rsiSetUserData__PvPv;
	long args[2];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (long) ppc.PP_Regs[0];
}
extern "ASM" void _PPCStub_stub_rsiCreateContext__PPv(void);
extern "ASM" long stub_rsiCreateContext__PPv(void *arg1)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_rsiCreateContext__PPv)(void) = _PPCStub_stub_rsiCreateContext__PPv;
	ppc.PP_Code = _PPCData_stub_rsiCreateContext__PPv;
	long args[1];
	args[0] = (long) arg1;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (long) ppc.PP_Regs[0];
}
extern "ASM" void _PPCStub_stub_rsiFreeContext__Pv(void);
extern "ASM" long stub_rsiFreeContext__Pv(void *arg1)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_rsiFreeContext__Pv)(void) = _PPCStub_stub_rsiFreeContext__Pv;
	ppc.PP_Code = _PPCData_stub_rsiFreeContext__Pv;
	long args[1];
	args[0] = (long) arg1;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (long) ppc.PP_Regs[0];
}
extern "ASM" void _PPCStub_stub_rsiSetCheckCancelCB__PvPFsPvp(void);
extern "ASM" long stub_rsiSetCheckCancelCB__PvPFsPvp(void *arg1,void *arg2)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_rsiSetCheckCancelCB__PvPFsPvp)(void) = _PPCStub_stub_rsiSetCheckCancelCB__PvPFsPvp;
	ppc.PP_Code = _PPCData_stub_rsiSetCheckCancelCB__PvPFsPvp;
	long args[2];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (long) ppc.PP_Regs[0];
}
extern "ASM" void _PPCStub_stub_rsiLoadObject__PvPce(void);
extern "ASM" long stub_rsiLoadObject__PvPce(void *arg1,void *arg2)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_rsiLoadObject__PvPce)(void) = _PPCStub_stub_rsiLoadObject__PvPce;
	ppc.PP_Code = _PPCData_stub_rsiLoadObject__PvPce;
	long args[3];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) ((&arg2)+1);
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (long) ppc.PP_Regs[0];
}
extern "ASM" void Cooperate__Pv(void *arg1);
extern "ASM" void __saveds _68kStub_Cooperate__Pv(register __a0 long *args)
{
	Cooperate__Pv(
		(void *) args[0]
	);
}
extern "ASM" short CheckCancel__Pv(void *arg1);
extern "ASM" short __saveds _68kStub_CheckCancel__Pv(register __a0 long *args)
{
	return CheckCancel__Pv(
		(void *) args[0]
	);
}
extern "ASM" void UpdateStatus__PvffiiP14rsiSMALL_COLOR(void *arg1,float arg2,float arg3,long arg4,long arg5,void *arg6);
extern "ASM" void __saveds _68kStub_UpdateStatus__PvffiiP14rsiSMALL_COLOR(register __a0 long *args)
{
	UpdateStatus__PvffiiP14rsiSMALL_COLOR(
		(void *) args[0],
		*((float *) &args[1]),
		*((float *) &args[2]),
		(long) args[3],
		(long) args[4],
		(void *) args[5]
	);
}
extern "ASM" void WriteLog__PvPc(void *arg1,void *arg2);
extern "ASM" void __saveds _68kStub_WriteLog__PvPc(register __a0 long *args)
{
	WriteLog__PvPc(
		(void *) args[0],
		(void *) args[1]
	);
}
extern "ASM" void _PPCStub_stub_rsiGetUserData__PvPPv(void);
extern "ASM" long stub_rsiGetUserData__PvPPv(void *arg1,void *arg2)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_rsiGetUserData__PvPPv)(void) = _PPCStub_stub_rsiGetUserData__PvPPv;
	ppc.PP_Code = _PPCData_stub_rsiGetUserData__PvPPv;
	long args[2];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (long) ppc.PP_Regs[0];
}
extern "ASM" void _PPCStub_stub_rsiSetObjectPath__PvPc(void);
extern "ASM" long stub_rsiSetObjectPath__PvPc(void *arg1,void *arg2)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_rsiSetObjectPath__PvPc)(void) = _PPCStub_stub_rsiSetObjectPath__PvPc;
	ppc.PP_Code = _PPCData_stub_rsiSetObjectPath__PvPc;
	long args[2];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (long) ppc.PP_Regs[0];
}
extern "ASM" void _PPCStub_stub_rsiSetBrushPath__PvPc(void);
extern "ASM" long stub_rsiSetBrushPath__PvPc(void *arg1,void *arg2)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_rsiSetBrushPath__PvPc)(void) = _PPCStub_stub_rsiSetBrushPath__PvPc;
	ppc.PP_Code = _PPCData_stub_rsiSetBrushPath__PvPc;
	long args[2];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (long) ppc.PP_Regs[0];
}
extern "ASM" void _PPCStub_stub_rsiSetRenderField__Pviiii(void);
extern "ASM" long stub_rsiSetRenderField__Pviiii(void *arg1,long arg2,long arg3,long arg4,long arg5)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_rsiSetRenderField__Pviiii)(void) = _PPCStub_stub_rsiSetRenderField__Pviiii;
	ppc.PP_Code = _PPCData_stub_rsiSetRenderField__Pviiii;
	long args[5];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) arg3;
	args[3] = (long) arg4;
	args[4] = (long) arg5;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (long) ppc.PP_Regs[0];
}
extern "ASM" void _PPCStub_stub_rsiSetTexturePath__PvPc(void);
extern "ASM" long stub_rsiSetTexturePath__PvPc(void *arg1,void *arg2)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_rsiSetTexturePath__PvPc)(void) = _PPCStub_stub_rsiSetTexturePath__PvPc;
	ppc.PP_Code = _PPCData_stub_rsiSetTexturePath__PvPc;
	long args[2];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (long) ppc.PP_Regs[0];
}
extern "ASM" void _PPCStub_stub_rsiRender__PvPPUce(void);
extern "ASM" long stub_rsiRender__PvPPUce(void *arg1,void *arg2)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_rsiRender__PvPPUce)(void) = _PPCStub_stub_rsiRender__PvPPUce;
	ppc.PP_Code = _PPCData_stub_rsiRender__PvPPUce;
	long args[3];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) ((&arg2)+1);
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (long) ppc.PP_Regs[0];
}
extern "ASM" void _PPCStub_stub_rsiSetAntialias__Pve(void);
extern "ASM" long stub_rsiSetAntialias__Pve(void *arg1)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_rsiSetAntialias__Pve)(void) = _PPCStub_stub_rsiSetAntialias__Pve;
	ppc.PP_Code = _PPCData_stub_rsiSetAntialias__Pve;
	long args[2];
	args[0] = (long) arg1;
	args[1] = (long) ((&arg1)+1);
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (long) ppc.PP_Regs[0];
}
extern "ASM" void _PPCStub_stub_rsiSetWorld__Pve(void);
extern "ASM" long stub_rsiSetWorld__Pve(void *arg1)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_rsiSetWorld__Pve)(void) = _PPCStub_stub_rsiSetWorld__Pve;
	ppc.PP_Code = _PPCData_stub_rsiSetWorld__Pve;
	long args[2];
	args[0] = (long) arg1;
	args[1] = (long) ((&arg1)+1);
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (long) ppc.PP_Regs[0];
}
extern "ASM" void _PPCStub_stub_rsiSetDistribution__Pvii(void);
extern "ASM" long stub_rsiSetDistribution__Pvii(void *arg1,long arg2,long arg3)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_rsiSetDistribution__Pvii)(void) = _PPCStub_stub_rsiSetDistribution__Pvii;
	ppc.PP_Code = _PPCData_stub_rsiSetDistribution__Pvii;
	long args[3];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) arg3;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (long) ppc.PP_Regs[0];
}
extern "ASM" void _PPCStub_stub_rsiSetScreen__Pvii(void);
extern "ASM" long stub_rsiSetScreen__Pvii(void *arg1,long arg2,long arg3)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_rsiSetScreen__Pvii)(void) = _PPCStub_stub_rsiSetScreen__Pvii;
	ppc.PP_Code = _PPCData_stub_rsiSetScreen__Pvii;
	long args[3];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) arg3;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (long) ppc.PP_Regs[0];
}
extern "ASM" void _PPCStub_stub_rsiSetOctreeDepth__Pvi(void);
extern "ASM" long stub_rsiSetOctreeDepth__Pvi(void *arg1,long arg2)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_rsiSetOctreeDepth__Pvi)(void) = _PPCStub_stub_rsiSetOctreeDepth__Pvi;
	ppc.PP_Code = _PPCData_stub_rsiSetOctreeDepth__Pvi;
	long args[2];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (long) ppc.PP_Regs[0];
}
extern "ASM" void _PPCStub_ppcSetCallbacks__Pv(void);
extern "ASM" long ppcSetCallbacks__Pv(void *arg1)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_ppcSetCallbacks__Pv)(void) = _PPCStub_ppcSetCallbacks__Pv;
	ppc.PP_Code = _PPCData_ppcSetCallbacks__Pv;
	long args[1];
	args[0] = (long) arg1;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (long) ppc.PP_Regs[0];
}
extern "ASM" void _PPCStub_stub_rsiCreateMesh__PvPvP09rsiVECTORP07rsiEDGEiP07rsiMESHie(void);
extern "ASM" long stub_rsiCreateMesh__PvPvP09rsiVECTORP07rsiEDGEiP07rsiMESHie(void *arg1,void *arg2,void *arg3,void *arg4,long arg5,void *arg6,long arg7)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_rsiCreateMesh__PvPvP09rsiVECTORP07rsiEDGEiP07rsiMESHie)(void) = _PPCStub_stub_rsiCreateMesh__PvPvP09rsiVECTORP07rsiEDGEiP07rsiMESHie;
	ppc.PP_Code = _PPCData_stub_rsiCreateMesh__PvPvP09rsiVECTORP07rsiEDGEiP07rsiMESHie;
	long args[8];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) arg3;
	args[3] = (long) arg4;
	args[4] = (long) arg5;
	args[5] = (long) arg6;
	args[6] = (long) arg7;
	args[7] = (long) ((&arg7)+1);
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (long) ppc.PP_Regs[0];
}

extern "ASM" void _PPCStub_stub_rsiCreateDirectionalLight__PvPPv(void);
extern "ASM" long stub_rsiCreateDirectionalLight__PvPPv(void *arg1,void *arg2)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_rsiCreateDirectionalLight__PvPPv)(void) = _PPCStub_stub_rsiCreateDirectionalLight__PvPPv;
	ppc.PP_Code = _PPCData_stub_rsiCreateDirectionalLight__PvPPv;
	long args[2];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (long) ppc.PP_Regs[0];
}

extern "ASM" void _PPCStub_stub_rsiSetDirectionalLight__PvPve(void);
extern "ASM" long stub_rsiSetDirectionalLight__PvPve(void *arg1,void *arg2)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_rsiSetDirectionalLight__PvPve)(void) = _PPCStub_stub_rsiSetDirectionalLight__PvPve;
	ppc.PP_Code = _PPCData_stub_rsiSetDirectionalLight__PvPve;
	long args[3];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) ((&arg2)+1);
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (long) ppc.PP_Regs[0];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// objhand stubs

extern "ASM" void *DirectionalLightAdd__Pv(void *arg1);
extern "ASM" void *__saveds _68kStub_DirectionalLightAdd__Pv(register __a0 long *args)
{
	return DirectionalLightAdd__Pv(
		(void *) args[0]
	);
}

extern "ASM" void *SpotLightAdd__Pv(void *arg1);
extern "ASM" void *__saveds _68kStub_SpotLightAdd__Pv(register __a0 long *args)
{
	return SpotLightAdd__Pv(
		(void *) args[0]
	);
}
extern "ASM" void *PointLightAdd__Pv(void *arg1);
extern "ASM" void *__saveds _68kStub_PointLightAdd__Pv(register __a0 long *args)
{
	return PointLightAdd__Pv(
		(void *) args[0]
	);
}
extern "ASM" short StarIntensities__PvP04STARiPf(void *arg1,void *arg2,long arg3,void *arg4);
extern "ASM" short __saveds _68kStub_StarIntensities__PvP04STARiPf(register __a0 long *args)
{
	return StarIntensities__PvP04STARiPf(
		(void *) args[0],
		(void *) args[1],
		(long) args[2],
		(void *) args[3]
	);
}
extern "ASM" void *StarAdd__PvP05LIGHT(void *arg1,void *arg2);
extern "ASM" void *__saveds _68kStub_StarAdd__PvP05LIGHT(register __a0 long *args)
{
	return StarAdd__PvP05LIGHT(
		(void *) args[0],
		(void *) args[1]
	);
}
extern "ASM" void *FlareAdd__PvP05LIGHT(void *arg1,void *arg2);
extern "ASM" void *__saveds _68kStub_FlareAdd__PvP05LIGHT(register __a0 long *args)
{
	return FlareAdd__PvP05LIGHT(
		(void *) args[0],
		(void *) args[1]
	);
}
extern "ASM" void *CameraAdd__Pv(void *arg1);
extern "ASM" void *__saveds _68kStub_CameraAdd__Pv(register __a0 long *args)
{
	return CameraAdd__Pv(
		(void *) args[0]
	);
}
extern "ASM" void BrushSize__PvP12BRUSH_OBJECTP06VECTOR(void *arg1,void *arg2,void *arg3);
extern "ASM" void __saveds _68kStub_BrushSize__PvP12BRUSH_OBJECTP06VECTOR(register __a0 long *args)
{
	BrushSize__PvP12BRUSH_OBJECTP06VECTOR(
		(void *) args[0],
		(void *) args[1],
		(void *) args[2]
	);
}
extern "ASM" void BrushOrient__PvP12BRUSH_OBJECTP06VECTORP06VECTORP06VECTOR(void *arg1,void *arg2,void *arg3,void *arg4,void *arg5);
extern "ASM" void __saveds _68kStub_BrushOrient__PvP12BRUSH_OBJECTP06VECTORP06VECTORP06VECTOR(register __a0 long *args)
{
	BrushOrient__PvP12BRUSH_OBJECTP06VECTORP06VECTORP06VECTOR(
		(void *) args[0],
		(void *) args[1],
		(void *) args[2],
		(void *) args[3],
		(void *) args[4]
	);
}
extern "ASM" void BrushPos__PvP12BRUSH_OBJECTP06VECTOR(void *arg1,void *arg2,void *arg3);
extern "ASM" void __saveds _68kStub_BrushPos__PvP12BRUSH_OBJECTP06VECTOR(register __a0 long *args)
{
	BrushPos__PvP12BRUSH_OBJECTP06VECTOR(
		(void *) args[0],
		(void *) args[1],
		(void *) args[2]
	);
}
extern "ASM" void *BrushObjectAdd__PvP06OBJECTPvP12BRUSH_OBJECT(void *arg1,void *arg2,void *arg3,void *arg4);
extern "ASM" void *__saveds _68kStub_BrushObjectAdd__PvP06OBJECTPvP12BRUSH_OBJECT(register __a0 long *args)
{
	return BrushObjectAdd__PvP06OBJECTPvP12BRUSH_OBJECT(
		(void *) args[0],
		(void *) args[1],
		(void *) args[2],
		(void *) args[3]
	);
}
extern "ASM" void *BrushAdd__PvPcP07SURFACE(void *arg1,void *arg2,void *arg3);
extern "ASM" void *__saveds _68kStub_BrushAdd__PvPcP07SURFACE(register __a0 long *args)
{
	return BrushAdd__PvPcP07SURFACE(
		(void *) args[0],
		(void *) args[1],
		(void *) args[2]
	);
}
extern "ASM" void TextureSize__PvP14TEXTURE_OBJECTP06VECTOR(void *arg1,void *arg2,void *arg3);
extern "ASM" void __saveds _68kStub_TextureSize__PvP14TEXTURE_OBJECTP06VECTOR(register __a0 long *args)
{
	TextureSize__PvP14TEXTURE_OBJECTP06VECTOR(
		(void *) args[0],
		(void *) args[1],
		(void *) args[2]
	);
}
extern "ASM" void *TextureObjectAdd__PvP06OBJECTPvP14TEXTURE_OBJECT(void *arg1,void *arg2,void *arg3,void *arg4);
extern "ASM" void *__saveds _68kStub_TextureObjectAdd__PvP06OBJECTPvP14TEXTURE_OBJECT(register __a0 long *args)
{
	return TextureObjectAdd__PvP06OBJECTPvP14TEXTURE_OBJECT(
		(void *) args[0],
		(void *) args[1],
		(void *) args[2],
		(void *) args[3]
	);
}
extern "ASM" short RSTextureParams__PvP07TEXTUREiPUc(void *arg1,void *arg2,long arg3,void *arg4);
extern "ASM" short __saveds _68kStub_RSTextureParams__PvP07TEXTUREiPUc(register __a0 long *args)
{
	return RSTextureParams__PvP07TEXTUREiPUc(
		(void *) args[0],
		(void *) args[1],
		(long) args[2],
		(void *) args[3]
	);
}
extern "ASM" void *RSTextureAdd__PvPcP07SURFACE(void *arg1,void *arg2,void *arg3);
extern "ASM" void *__saveds _68kStub_RSTextureAdd__PvPcP07SURFACE(register __a0 long *args)
{
	return RSTextureAdd__PvPcP07SURFACE(
		(void *) args[0],
		(void *) args[1],
		(void *) args[2]
	);
}
extern "ASM" void *ITextureAdd__PvPcP07SURFACE(void *arg1,void *arg2,void *arg3);
extern "ASM" void *__saveds _68kStub_ITextureAdd__PvPcP07SURFACE(register __a0 long *args)
{
	return ITextureAdd__PvPcP07SURFACE(
		(void *) args[0],
		(void *) args[1],
		(void *) args[2]
	);
}
extern "ASM" short SORPoints__PvP06OBJECTP06VECT2DUj(void *arg1,void *arg2,void *arg3,unsigned long arg4);
extern "ASM" short __saveds _68kStub_SORPoints__PvP06OBJECTP06VECT2DUj(register __a0 long *args)
{
	return SORPoints__PvP06OBJECTP06VECT2DUj(
		(void *) args[0],
		(void *) args[1],
		(void *) args[2],
		(unsigned long) args[3]
	);
}
extern "ASM" void *SORAdd__Pv(void *arg1);
extern "ASM" void *__saveds _68kStub_SORAdd__Pv(register __a0 long *args)
{
	return SORAdd__Pv(
		(void *) args[0]
	);
}
extern "ASM" void *CSGAdd__Pv(void *arg1);
extern "ASM" void *__saveds _68kStub_CSGAdd__Pv(register __a0 long *args)
{
	return CSGAdd__Pv(
		(void *) args[0]
	);
}
extern "ASM" void *PlaneAdd__Pv(void *arg1);
extern "ASM" void *__saveds _68kStub_PlaneAdd__Pv(register __a0 long *args)
{
	return PlaneAdd__Pv(
		(void *) args[0]
	);
}
extern "ASM" void *ConeAdd__Pv(void *arg1);
extern "ASM" void *__saveds _68kStub_ConeAdd__Pv(register __a0 long *args)
{
	return ConeAdd__Pv(
		(void *) args[0]
	);
}
extern "ASM" void *CylinderAdd__Pv(void *arg1);
extern "ASM" void *__saveds _68kStub_CylinderAdd__Pv(register __a0 long *args)
{
	return CylinderAdd__Pv(
		(void *) args[0]
	);
}
extern "ASM" void *BoxAdd__Pv(void *arg1);
extern "ASM" void *__saveds _68kStub_BoxAdd__Pv(register __a0 long *args)
{
	return BoxAdd__Pv(
		(void *) args[0]
	);
}
extern "ASM" short MeshAddScenario__PvP06OBJECTP06VECTORiP09LINK_EDGEiP09LINK_MESHiP06VECTORs(void *arg1,void *arg2,void *arg3,long arg4,void *arg5,long arg6,void *arg7,long arg8,void *arg9,short arg10);
extern "ASM" short __saveds _68kStub_MeshAddScenario__PvP06OBJECTP06VECTORiP09LINK_EDGEiP09LINK_MESHiP06VECTORs(register __a0 long *args)
{
	return MeshAddScenario__PvP06OBJECTP06VECTORiP09LINK_EDGEiP09LINK_MESHiP06VECTORs(
		(void *) args[0],
		(void *) args[1],
		(void *) args[2],
		(long) args[3],
		(void *) args[4],
		(long) args[5],
		(void *) args[6],
		(long) args[7],
		(void *) args[8],
		(short) args[9]
	);
}
extern "ASM" void *MeshCreate__Pv(void *arg1);
extern "ASM" void *__saveds _68kStub_MeshCreate__Pv(register __a0 long *args)
{
	return MeshCreate__Pv(
		(void *) args[0]
	);
}
extern "ASM" void TriangleSetEdgeFlags__PvPUc(void *arg1,void *arg2);
extern "ASM" void __saveds _68kStub_TriangleSetEdgeFlags__PvPUc(register __a0 long *args)
{
	TriangleSetEdgeFlags__PvPUc(
		(void *) args[0],
		(void *) args[1]
	);
}
extern "ASM" short TriangleAddEdges__PviPUs(void *arg1,long arg2,void *arg3);
extern "ASM" short __saveds _68kStub_TriangleAddEdges__PviPUs(register __a0 long *args)
{
	return TriangleAddEdges__PviPUs(
		(void *) args[0],
		(long) args[1],
		(void *) args[2]
	);
}
extern "ASM" short TriangleAddPoints__PviP06VECTOR(void *arg1,long arg2,void *arg3);
extern "ASM" short __saveds _68kStub_TriangleAddPoints__PviP06VECTOR(register __a0 long *args)
{
	return TriangleAddPoints__PviP06VECTOR(
		(void *) args[0],
		(long) args[1],
		(void *) args[2]
	);
}
extern "ASM" void *TriangleAdd__PviP07SURFACEPv(void *arg1,long arg2,void *arg3,void *arg4);
extern "ASM" void *__saveds _68kStub_TriangleAdd__PviP07SURFACEPv(register __a0 long *args)
{
	return TriangleAdd__PviP07SURFACEPv(
		(void *) args[0],
		(long) args[1],
		(void *) args[2],
		(void *) args[3]
	);
}
extern "ASM" void *SphereAdd__Pv(void *arg1);
extern "ASM" void *__saveds _68kStub_SphereAdd__Pv(register __a0 long *args)
{
	return SphereAdd__Pv(
		(void *) args[0]
	);
}
extern "ASM" short SurfaceCopy__PvP07SURFACEP07SURFACE(void *arg1,void *arg2,void *arg3);
extern "ASM" short __saveds _68kStub_SurfaceCopy__PvP07SURFACEP07SURFACE(register __a0 long *args)
{
	return SurfaceCopy__PvP07SURFACEP07SURFACE(
		(void *) args[0],
		(void *) args[1],
		(void *) args[2]
	);
}
extern "ASM" void *SurfaceGetByName__PvPc(void *arg1,void *arg2);
extern "ASM" void *__saveds _68kStub_SurfaceGetByName__PvPc(register __a0 long *args)
{
	return SurfaceGetByName__PvPc(
		(void *) args[0],
		(void *) args[1]
	);
}
extern "ASM" void *SurfaceAdd__Pv(void *arg1);
extern "ASM" void *__saveds _68kStub_SurfaceAdd__Pv(register __a0 long *args)
{
	return SurfaceAdd__Pv(
		(void *) args[0]
	);
}
extern "ASM" short ObjectTrack__PvP06OBJECTPc(void *arg1,void *arg2,void *arg3);
extern "ASM" short __saveds _68kStub_ObjectTrack__PvP06OBJECTPc(register __a0 long *args)
{
	return ObjectTrack__PvP06OBJECTPc(
		(void *) args[0],
		(void *) args[1],
		(void *) args[2]
	);
}
extern "ASM" void ObjectAxis__PvP06OBJECTP06VECTORP06VECTORP06VECTORP06VECTORP06VECTOR(void *arg1,void *arg2,void *arg3,void *arg4,void *arg5,void *arg6,void *arg7);
extern "ASM" void __saveds _68kStub_ObjectAxis__PvP06OBJECTP06VECTORP06VECTORP06VECTORP06VECTORP06VECTOR(register __a0 long *args)
{
	ObjectAxis__PvP06OBJECTP06VECTORP06VECTORP06VECTORP06VECTORP06VECTOR(
		(void *) args[0],
		(void *) args[1],
		(void *) args[2],
		(void *) args[3],
		(void *) args[4],
		(void *) args[5],
		(void *) args[6]
	);
}
extern "ASM" short ObjectName__PvP06OBJECTPc(void *arg1,void *arg2,void *arg3);
extern "ASM" short __saveds _68kStub_ObjectName__PvP06OBJECTPc(register __a0 long *args)
{
	return ObjectName__PvP06OBJECTPc(
		(void *) args[0],
		(void *) args[1],
		(void *) args[2]
	);
}
extern "ASM" void ObjectEnd__Pv(void *arg1);
extern "ASM" void __saveds _68kStub_ObjectEnd__Pv(register __a0 long *args)
{
	ObjectEnd__Pv(
		(void *) args[0]
	);
}
extern "ASM" short SurfaceName__PvP07SURFACEPc(void *arg1,void *arg2,void *arg3);
extern "ASM" short __saveds _68kStub_SurfaceName__PvP07SURFACEPc(register __a0 long *args)
{
	return SurfaceName__PvP07SURFACEPc(
		(void *) args[0],
		(void *) args[1],
		(void *) args[2]
	);
}
extern "ASM" void _PPCStub_ppc_objCleanup_(void);
extern "ASM" void ppc_objCleanup_()
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_ppc_objCleanup_)(void) = _PPCStub_ppc_objCleanup_;
	ppc.PP_Code = _PPCData_ppc_objCleanup_;
	RunPPC(&ppc);
}
extern "ASM" void _PPCStub_ppc_objInit_(void);
extern "ASM" short ppc_objInit_()
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_ppc_objInit_)(void) = _PPCStub_ppc_objInit_;
	ppc.PP_Code = _PPCData_ppc_objInit_;
	RunPPC(&ppc);
	return (short) ppc.PP_Regs[0];
}
extern "ASM" void _PPCStub__ppc_objRead(void);
extern "ASM" void *_ppc_objRead(void *arg1,void *arg2,void *arg3,void *arg4,void *arg5,void *arg6,void *arg7,void *arg8,void *arg9,void *arg10,void *arg11)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData__ppc_objRead)(void) = _PPCStub__ppc_objRead;
	ppc.PP_Code = _PPCData__ppc_objRead;
	long args[11];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) arg3;
	args[3] = (long) arg4;
	args[4] = (long) arg5;
	args[5] = (long) arg6;
	args[6] = (long) arg7;
	args[7] = (long) arg8;
	args[8] = (long) arg9;
	args[9] = (long) arg10;
	args[10] = (long) arg11;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (void *) ppc.PP_Regs[0];
}

// OpenGL stubs -------------------------------------------------------------------------------------

extern "ASM" void _PPCStub_stub_glGetFloatv__EPf(void);
extern "ASM" void stub_glGetFloatv__EPf(long arg1,void *arg2)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_glGetFloatv__EPf)(void) = _PPCStub_stub_glGetFloatv__EPf;
	ppc.PP_Code = _PPCData_stub_glGetFloatv__EPf;
	long args[2];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
}

extern "ASM" void _PPCStub_OpenGLDrawPointLight__P06VECTOR(void);
extern "ASM" void OpenGLDrawPointLight__P06VECTOR(void *arg1)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_OpenGLDrawPointLight__P06VECTOR)(void) = _PPCStub_OpenGLDrawPointLight__P06VECTOR;
	ppc.PP_Code = _PPCData_OpenGLDrawPointLight__P06VECTOR;
	long args[1];
	args[0] = (long) arg1;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
}

extern "ASM" void _PPCStub_stub_glPopMatrix_(void);
extern "ASM" void stub_glPopMatrix_()
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_glPopMatrix_)(void) = _PPCStub_stub_glPopMatrix_;
	ppc.PP_Code = _PPCData_stub_glPopMatrix_;
	RunPPC(&ppc);
}

extern "ASM" void _PPCStub_stub_AmigaMesaDestroyContext__P17amigamesa_context(void);
extern "ASM" void stub_AmigaMesaDestroyContext__P17amigamesa_context(void *arg1)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_AmigaMesaDestroyContext__P17amigamesa_context)(void) = _PPCStub_stub_AmigaMesaDestroyContext__P17amigamesa_context;
	ppc.PP_Code = _PPCData_stub_AmigaMesaDestroyContext__P17amigamesa_context;
	long args[1];
	args[0] = (long) arg1;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
}

extern "ASM" void _PPCStub_OpenGLDrawPlane__P06VECTOR(void);
extern "ASM" void OpenGLDrawPlane__P06VECTOR(void *arg1)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_OpenGLDrawPlane__P06VECTOR)(void) = _PPCStub_OpenGLDrawPlane__P06VECTOR;
	ppc.PP_Code = _PPCData_OpenGLDrawPlane__P06VECTOR;
	long args[1];
	args[0] = (long) arg1;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
}

extern "ASM" void _PPCStub_stub_glVertex3fv__PCf(void);
extern "ASM" void stub_glVertex3fv__PCf(void *arg1)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_glVertex3fv__PCf)(void) = _PPCStub_stub_glVertex3fv__PCf;
	ppc.PP_Code = _PPCData_stub_glVertex3fv__PCf;
	long args[1];
	args[0] = (long) arg1;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
}

extern "ASM" void _PPCStub_OGLLoadMatrix__P06MATRIX(void);
extern "ASM" void OGLLoadMatrix__P06MATRIX(void *arg1)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_OGLLoadMatrix__P06MATRIX)(void) = _PPCStub_OGLLoadMatrix__P06MATRIX;
	ppc.PP_Code = _PPCData_OGLLoadMatrix__P06MATRIX;
	long args[1];
	args[0] = (long) arg1;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
}

extern "ASM" void _PPCStub_stub_AmigaMesaMakeCurrent__P17amigamesa_contextP16amigamesa_buffer(void);
extern "ASM" void stub_AmigaMesaMakeCurrent__P17amigamesa_contextP16amigamesa_buffer(void *arg1,void *arg2)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_AmigaMesaMakeCurrent__P17amigamesa_contextP16amigamesa_buffer)(void) = _PPCStub_stub_AmigaMesaMakeCurrent__P17amigamesa_contextP16amigamesa_buffer;
	ppc.PP_Code = _PPCData_stub_AmigaMesaMakeCurrent__P17amigamesa_contextP16amigamesa_buffer;
	long args[2];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
}

extern "ASM" void _PPCStub_OpenGLDrawSpotLight__P06VECTORP06VECTORf(void);
extern "ASM" void OpenGLDrawSpotLight__P06VECTORP06VECTORf(void *arg1,void *arg2,float arg3)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_OpenGLDrawSpotLight__P06VECTORP06VECTORf)(void) = _PPCStub_OpenGLDrawSpotLight__P06VECTORP06VECTORf;
	ppc.PP_Code = _PPCData_OpenGLDrawSpotLight__P06VECTORP06VECTORf;
	long args[3];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	*((float *) &args[2]) = arg3;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
}

extern "ASM" void _PPCStub_OpenGLDrawBox__P06VECTORP06VECTOR(void);
extern "ASM" void OpenGLDrawBox__P06VECTORP06VECTOR(void *arg1,void *arg2)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_OpenGLDrawBox__P06VECTORP06VECTOR)(void) = _PPCStub_OpenGLDrawBox__P06VECTORP06VECTOR;
	ppc.PP_Code = _PPCData_OpenGLDrawBox__P06VECTORP06VECTOR;
	long args[2];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
}

extern "ASM" void _PPCStub_OpenGLDrawCamera__P07DISPLAYffffffP06VECTOR(void);
extern "ASM" void OpenGLDrawCamera__P07DISPLAYffffffP06VECTOR(void *arg1,float arg2,float arg3,float arg4,float arg5,float arg6,float arg7,void *arg8)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_OpenGLDrawCamera__P07DISPLAYffffffP06VECTOR)(void) = _PPCStub_OpenGLDrawCamera__P07DISPLAYffffffP06VECTOR;
	ppc.PP_Code = _PPCData_OpenGLDrawCamera__P07DISPLAYffffffP06VECTOR;
	long args[8];
	args[0] = (long) arg1;
	*((float *) &args[1]) = arg2;
	*((float *) &args[2]) = arg3;
	*((float *) &args[3]) = arg4;
	*((float *) &args[4]) = arg5;
	*((float *) &args[5]) = arg6;
	*((float *) &args[6]) = arg7;
	args[7] = (long) arg8;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
}

extern "ASM" void _PPCStub_OpenGLDrawCone__P06VECTORUj(void);
extern "ASM" void OpenGLDrawCone__P06VECTORUj(void *arg1,unsigned long arg2)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_OpenGLDrawCone__P06VECTORUj)(void) = _PPCStub_OpenGLDrawCone__P06VECTORUj;
	ppc.PP_Code = _PPCData_OpenGLDrawCone__P06VECTORUj;
	long args[2];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
}

extern "ASM" void _PPCStub_stub_glClear__Ui(void);
extern "ASM" void stub_glClear__Ui(unsigned long arg1)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_glClear__Ui)(void) = _PPCStub_stub_glClear__Ui;
	ppc.PP_Code = _PPCData_stub_glClear__Ui;
	long args[1];
	args[0] = (long) arg1;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
}

extern "ASM" void _PPCStub_stub_glLightfv__EEPCf(void);
extern "ASM" void stub_glLightfv__EEPCf(long arg1,long arg2,void *arg3)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_glLightfv__EEPCf)(void) = _PPCStub_stub_glLightfv__EEPCf;
	ppc.PP_Code = _PPCData_stub_glLightfv__EEPCf;
	long args[3];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) arg3;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
}

extern "ASM" void _PPCStub_stub_glViewport__iiii(void);
extern "ASM" void stub_glViewport__iiii(long arg1,long arg2,long arg3,long arg4)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_glViewport__iiii)(void) = _PPCStub_stub_glViewport__iiii;
	ppc.PP_Code = _PPCData_stub_glViewport__iiii;
	long args[4];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) arg3;
	args[3] = (long) arg4;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
}

extern "ASM" void _PPCStub_stub_gluLookAt__ddddddddd(void);
extern "ASM" void stub_gluLookAt__ddddddddd(double arg1,double arg2,double arg3,double arg4,double arg5,double arg6,double arg7,double arg8,double arg9)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_gluLookAt__ddddddddd)(void) = _PPCStub_stub_gluLookAt__ddddddddd;
	ppc.PP_Code = _PPCData_stub_gluLookAt__ddddddddd;
	long args[18];
	*((double *) &args[0]) = arg1;
	*((double *) &args[2]) = arg2;
	*((double *) &args[4]) = arg3;
	*((double *) &args[6]) = arg4;
	*((double *) &args[8]) = arg5;
	*((double *) &args[10]) = arg6;
	*((double *) &args[12]) = arg7;
	*((double *) &args[14]) = arg8;
	*((double *) &args[16]) = arg9;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
}

extern "ASM" void _PPCStub_stub_glEnable__E(void);
extern "ASM" void stub_glEnable__E(long arg1)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_glEnable__E)(void) = _PPCStub_stub_glEnable__E;
	ppc.PP_Code = _PPCData_stub_glEnable__E;
	long args[1];
	args[0] = (long) arg1;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
}

extern "ASM" void _PPCStub_stub_AmigaMesaSwapBuffers__P17amigamesa_context(void);
extern "ASM" void stub_AmigaMesaSwapBuffers__P17amigamesa_context(void *arg1)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_AmigaMesaSwapBuffers__P17amigamesa_context)(void) = _PPCStub_stub_AmigaMesaSwapBuffers__P17amigamesa_context;
	ppc.PP_Code = _PPCData_stub_AmigaMesaSwapBuffers__P17amigamesa_context;
	long args[1];
	args[0] = (long) arg1;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
}

extern "ASM" void _PPCStub_stub_glLoadMatrixf__PCf(void);
extern "ASM" void stub_glLoadMatrixf__PCf(void *arg1)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_glLoadMatrixf__PCf)(void) = _PPCStub_stub_glLoadMatrixf__PCf;
	ppc.PP_Code = _PPCData_stub_glLoadMatrixf__PCf;
	long args[1];
	args[0] = (long) arg1;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
}

extern "ASM" void _PPCStub_stub_glGetIntegerv__EPi(void);
extern "ASM" void stub_glGetIntegerv__EPi(long arg1,void *arg2)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_glGetIntegerv__EPi)(void) = _PPCStub_stub_glGetIntegerv__EPi;
	ppc.PP_Code = _PPCData_stub_glGetIntegerv__EPi;
	long args[2];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
}

extern "ASM" void _PPCStub_stub_glLightModeli__Ei(void);
extern "ASM" void stub_glLightModeli__Ei(long arg1,long arg2)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_glLightModeli__Ei)(void) = _PPCStub_stub_glLightModeli__Ei;
	ppc.PP_Code = _PPCData_stub_glLightModeli__Ei;
	long args[2];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
}

extern "ASM" void _PPCStub_stub_glColor3fv__Pf(void);
extern "ASM" void stub_glColor3fv__Pf(void *arg1)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_glColor3fv__Pf)(void) = _PPCStub_stub_glColor3fv__Pf;
	ppc.PP_Code = _PPCData_stub_glColor3fv__Pf;
	long args[1];
	args[0] = (long) arg1;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
}

extern "ASM" void _PPCStub_OpenGLDrawCylinder__P06VECTORUj(void);
extern "ASM" void OpenGLDrawCylinder__P06VECTORUj(void *arg1,unsigned long arg2)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_OpenGLDrawCylinder__P06VECTORUj)(void) = _PPCStub_OpenGLDrawCylinder__P06VECTORUj;
	ppc.PP_Code = _PPCData_OpenGLDrawCylinder__P06VECTORUj;
	long args[2];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
}

extern "ASM" void _PPCStub_stub_glPushMatrix_(void);
extern "ASM" void stub_glPushMatrix_()
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_glPushMatrix_)(void) = _PPCStub_stub_glPushMatrix_;
	ppc.PP_Code = _PPCData_stub_glPushMatrix_;
	RunPPC(&ppc);
}

extern "ASM" void _PPCStub_stub_glLoadIdentity_(void);
extern "ASM" void stub_glLoadIdentity_()
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_glLoadIdentity_)(void) = _PPCStub_stub_glLoadIdentity_;
	ppc.PP_Code = _PPCData_stub_glLoadIdentity_;
	RunPPC(&ppc);
}

extern "ASM" void _PPCStub_stub_glDisable__E(void);
extern "ASM" void stub_glDisable__E(long arg1)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_glDisable__E)(void) = _PPCStub_stub_glDisable__E;
	ppc.PP_Code = _PPCData_stub_glDisable__E;
	long args[1];
	args[0] = (long) arg1;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
}

extern "ASM" void _PPCStub_stub_glMatrixMode__E(void);
extern "ASM" void stub_glMatrixMode__E(long arg1)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_glMatrixMode__E)(void) = _PPCStub_stub_glMatrixMode__E;
	ppc.PP_Code = _PPCData_stub_glMatrixMode__E;
	long args[1];
	args[0] = (long) arg1;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
}

extern "ASM" void _PPCStub_stub_glClearColor__ffff(void);
extern "ASM" void stub_glClearColor__ffff(float arg1,float arg2,float arg3,float arg4)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_glClearColor__ffff)(void) = _PPCStub_stub_glClearColor__ffff;
	ppc.PP_Code = _PPCData_stub_glClearColor__ffff;
	long args[4];
	*((float *) &args[0]) = arg1;
	*((float *) &args[1]) = arg2;
	*((float *) &args[2]) = arg3;
	*((float *) &args[3]) = arg4;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
}

extern "ASM" void _PPCStub_stub_glBegin__E(void);
extern "ASM" void stub_glBegin__E(long arg1)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_glBegin__E)(void) = _PPCStub_stub_glBegin__E;
	ppc.PP_Code = _PPCData_stub_glBegin__E;
	long args[1];
	args[0] = (long) arg1;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
}

extern "ASM" void _PPCStub_stub_glLineStipple__iUs(void);
extern "ASM" void stub_glLineStipple__iUs(long arg1,unsigned short arg2)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_glLineStipple__iUs)(void) = _PPCStub_stub_glLineStipple__iUs;
	ppc.PP_Code = _PPCData_stub_glLineStipple__iUs;
	long args[2];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
}

extern "ASM" void _PPCStub_stub_glVertex3f__fff(void);
extern "ASM" void stub_glVertex3f__fff(float arg1,float arg2,float arg3)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_glVertex3f__fff)(void) = _PPCStub_stub_glVertex3f__fff;
	ppc.PP_Code = _PPCData_stub_glVertex3f__fff;
	long args[3];
	*((float *) &args[0]) = arg1;
	*((float *) &args[1]) = arg2;
	*((float *) &args[2]) = arg3;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
}

extern "ASM" void _PPCStub_stub_gluPerspective__dddd(void);
extern "ASM" void stub_gluPerspective__dddd(double arg1,double arg2,double arg3,double arg4)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_gluPerspective__dddd)(void) = _PPCStub_stub_gluPerspective__dddd;
	ppc.PP_Code = _PPCData_stub_gluPerspective__dddd;
	long args[8];
	*((double *) &args[0]) = arg1;
	*((double *) &args[2]) = arg2;
	*((double *) &args[4]) = arg3;
	*((double *) &args[6]) = arg4;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
}

extern "ASM" void _PPCStub_OpenGLDrawSphere__f(void);
extern "ASM" void OpenGLDrawSphere__f(float arg1)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_OpenGLDrawSphere__f)(void) = _PPCStub_OpenGLDrawSphere__f;
	ppc.PP_Code = _PPCData_OpenGLDrawSphere__f;
	long args[1];
	*((float *) &args[0]) = arg1;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
}

extern "ASM" void _PPCStub_stub_glOrtho__dddddd(void);
extern "ASM" void stub_glOrtho__dddddd(double arg1,double arg2,double arg3,double arg4,double arg5,double arg6)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_glOrtho__dddddd)(void) = _PPCStub_stub_glOrtho__dddddd;
	ppc.PP_Code = _PPCData_stub_glOrtho__dddddd;
	long args[12];
	*((double *) &args[0]) = arg1;
	*((double *) &args[2]) = arg2;
	*((double *) &args[4]) = arg3;
	*((double *) &args[6]) = arg4;
	*((double *) &args[8]) = arg5;
	*((double *) &args[10]) = arg6;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
}

extern "ASM" void _PPCStub_stub_glEnd_(void);
extern "ASM" void stub_glEnd_()
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_glEnd_)(void) = _PPCStub_stub_glEnd_;
	ppc.PP_Code = _PPCData_stub_glEnd_;
	RunPPC(&ppc);
}

extern "ASM" void _PPCStub_stub_glColorMaterial__EE(void);
extern "ASM" void stub_glColorMaterial__EE(long arg1,long arg2)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_glColorMaterial__EE)(void) = _PPCStub_stub_glColorMaterial__EE;
	ppc.PP_Code = _PPCData_stub_glColorMaterial__EE;
	long args[2];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
}

extern "ASM" void _PPCStub_stub_glVertex2f__ff(void);
extern "ASM" void stub_glVertex2f__ff(float arg1,float arg2)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_glVertex2f__ff)(void) = _PPCStub_stub_glVertex2f__ff;
	ppc.PP_Code = _PPCData_stub_glVertex2f__ff;
	long args[2];
	*((float *) &args[0]) = arg1;
	*((float *) &args[1]) = arg2;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
}

extern "ASM" void _PPCStub_stub_glMaterialf__EEf(void);
extern "ASM" void stub_glMaterialf__EEf(long arg1,long arg2,float arg3)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_glMaterialf__EEf)(void) = _PPCStub_stub_glMaterialf__EEf;
	ppc.PP_Code = _PPCData_stub_glMaterialf__EEf;
	long args[3];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	*((float *) &args[2]) = arg3;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
}

extern "ASM" void _PPCStub_stub_glMaterialfv__EEPCf(void);
extern "ASM" void stub_glMaterialfv__EEPCf(long arg1,long arg2,void *arg3)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_glMaterialfv__EEPCf)(void) = _PPCStub_stub_glMaterialfv__EEPCf;
	ppc.PP_Code = _PPCData_stub_glMaterialfv__EEPCf;
	long args[3];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) arg3;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
}

extern "ASM" void _PPCStub_OpenGLDrawSelBox__P06VECTORP06VECTORP06VECTORP06VECTOR(void);
extern "ASM" void OpenGLDrawSelBox__P06VECTORP06VECTORP06VECTORP06VECTOR(void *arg1,void *arg2,void *arg3,void *arg4)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_OpenGLDrawSelBox__P06VECTORP06VECTORP06VECTORP06VECTOR)(void) = _PPCStub_OpenGLDrawSelBox__P06VECTORP06VECTORP06VECTORP06VECTOR;
	ppc.PP_Code = _PPCData_OpenGLDrawSelBox__P06VECTORP06VECTORP06VECTORP06VECTOR;
	long args[4];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) arg3;
	args[3] = (long) arg4;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
}

extern "ASM" void _PPCStub_stub_AmigaMesaCreateContext__P07TagItem(void);
extern "ASM" void *stub_AmigaMesaCreateContext__P07TagItem(void *arg1)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_stub_AmigaMesaCreateContext__P07TagItem)(void) = _PPCStub_stub_AmigaMesaCreateContext__P07TagItem;
	ppc.PP_Code = _PPCData_stub_AmigaMesaCreateContext__P07TagItem;
	long args[1];
	args[0] = (long) arg1;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
	return (void *) ppc.PP_Regs[0];
}

extern "ASM" void _PPCStub_OpenGLDrawGrid__P07DISPLAY(void);
extern "ASM" void OpenGLDrawGrid__P07DISPLAY(void *arg1)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_OpenGLDrawGrid__P07DISPLAY)(void) = _PPCStub_OpenGLDrawGrid__P07DISPLAY;
	ppc.PP_Code = _PPCData_OpenGLDrawGrid__P07DISPLAY;
	long args[1];
	args[0] = (long) arg1;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
}

extern "ASM" void _PPCStub_SetGLColor__i(void);
extern "ASM" void SetGLColor__i(long arg1)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_SetGLColor__i)(void) = _PPCStub_SetGLColor__i;
	ppc.PP_Code = _PPCData_SetGLColor__i;
	long args[1];
	args[0] = (long) arg1;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
}

extern "ASM" void _PPCStub_OpenGLDrawDirectionalLight__P06VECTOR(void);
extern "ASM" void OpenGLDrawDirectionalLight__P06VECTOR(void *arg1)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_OpenGLDrawDirectionalLight__P06VECTOR)(void) = _PPCStub_OpenGLDrawDirectionalLight__P06VECTOR;
	ppc.PP_Code = _PPCData_OpenGLDrawDirectionalLight__P06VECTOR;
	long args[1];
	args[0] = (long) arg1;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
}

extern "ASM" void _PPCStub_OpenGLDrawRenderLines__P07DISPLAYP09RECTANGLEs(void);
extern "ASM" void OpenGLDrawRenderLines__P07DISPLAYP09RECTANGLEs(void *arg1,void *arg2,short arg3)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_OpenGLDrawRenderLines__P07DISPLAYP09RECTANGLEs)(void) = _PPCStub_OpenGLDrawRenderLines__P07DISPLAYP09RECTANGLEs;
	ppc.PP_Code = _PPCData_OpenGLDrawRenderLines__P07DISPLAYP09RECTANGLEs;
	long args[3];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) arg3;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
}

extern "ASM" void _PPCStub_OpenGLSetupFront__P07DISPLAYP05COLOR(void);
extern "ASM" void OpenGLSetupFront__P07DISPLAYP05COLOR(void *arg1,void *arg2)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_OpenGLSetupFront__P07DISPLAYP05COLOR)(void) = _PPCStub_OpenGLSetupFront__P07DISPLAYP05COLOR;
	ppc.PP_Code = _PPCData_OpenGLSetupFront__P07DISPLAYP05COLOR;
	long args[2];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
}

extern "ASM" void _PPCStub_OpenGLSetupTop__P07DISPLAYP05COLOR(void);
extern "ASM" void OpenGLSetupTop__P07DISPLAYP05COLOR(void *arg1,void *arg2)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_OpenGLSetupTop__P07DISPLAYP05COLOR)(void) = _PPCStub_OpenGLSetupTop__P07DISPLAYP05COLOR;
	ppc.PP_Code = _PPCData_OpenGLSetupTop__P07DISPLAYP05COLOR;
	long args[2];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
}

extern "ASM" void _PPCStub_OpenGLSetupPerspective__P07DISPLAYP05COLORP06VECTORP06VECTORP06VECTORff(void);
extern "ASM" void OpenGLSetupPerspective__P07DISPLAYP05COLORP06VECTORP06VECTORP06VECTORff(void *arg1,void *arg2,void *arg3,void *arg4,void *arg5,float arg6,float arg7)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_OpenGLSetupPerspective__P07DISPLAYP05COLORP06VECTORP06VECTORP06VECTORff)(void) = _PPCStub_OpenGLSetupPerspective__P07DISPLAYP05COLORP06VECTORP06VECTORP06VECTORff;
	ppc.PP_Code = _PPCData_OpenGLSetupPerspective__P07DISPLAYP05COLORP06VECTORP06VECTORP06VECTORff;
	long args[7];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) arg3;
	args[3] = (long) arg4;
	args[4] = (long) arg5;
	*((float *) &args[5]) = arg6;
	*((float *) &args[6]) = arg7;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
}

extern "ASM" void _PPCStub_OpenGLSetupRight__P07DISPLAYP05COLOR(void);
extern "ASM" void OpenGLSetupRight__P07DISPLAYP05COLOR(void *arg1,void *arg2)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_OpenGLSetupRight__P07DISPLAYP05COLOR)(void) = _PPCStub_OpenGLSetupRight__P07DISPLAYP05COLOR;
	ppc.PP_Code = _PPCData_OpenGLSetupRight__P07DISPLAYP05COLOR;
	long args[2];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
}

extern "ASM" void _PPCStub_OpenGLSetupCamera__P07DISPLAYP05COLORP06VECTORP06VECTORP06VECTORff(void);
extern "ASM" void OpenGLSetupCamera__P07DISPLAYP05COLORP06VECTORP06VECTORP06VECTORff(void *arg1,void *arg2,void *arg3,void *arg4,void *arg5,float arg6,float arg7)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_OpenGLSetupCamera__P07DISPLAYP05COLORP06VECTORP06VECTORP06VECTORff)(void) = _PPCStub_OpenGLSetupCamera__P07DISPLAYP05COLORP06VECTORP06VECTORP06VECTORff;
	ppc.PP_Code = _PPCData_OpenGLSetupCamera__P07DISPLAYP05COLORP06VECTORP06VECTORP06VECTORff;
	long args[7];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) arg3;
	args[3] = (long) arg4;
	args[4] = (long) arg5;
	*((float *) &args[5]) = arg6;
	*((float *) &args[6]) = arg7;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
}

extern "ASM" void _PPCStub_OpenGLDrawMesh__P08TRIANGLEP04EDGEP06VECTORP06VECTORUj(void);
extern "ASM" void OpenGLDrawMesh__P08TRIANGLEP04EDGEP06VECTORP06VECTORUj(void *arg1,void *arg2,void *arg3,void *arg4,unsigned long arg5)
{
	struct PPCArgs ppc = { };
	static void (*_PPCData_OpenGLDrawMesh__P08TRIANGLEP04EDGEP06VECTORP06VECTORUj)(void) = _PPCStub_OpenGLDrawMesh__P08TRIANGLEP04EDGEP06VECTORP06VECTORUj;
	ppc.PP_Code = _PPCData_OpenGLDrawMesh__P08TRIANGLEP04EDGEP06VECTORP06VECTORUj;
	long args[5];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) arg3;
	args[3] = (long) arg4;
	args[4] = (long) arg5;
	ppc.PP_Regs[0] = (ULONG) args;
	RunPPC(&ppc);
}




