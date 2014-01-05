#include <clib/exec_protos.h>
#include <stdarg.h>
#include <libraries/powerpc.h>
#include <pragma/powerpc_lib.h>

#include "typedefs.h"
#include "rsi.h"
#include "rsi_lib.h"
#include "errors.h"

extern struct Library *RayStormBase;
extern struct Library *PowerPCBase;

/*#define RSILIBRARY RayStormBase,
#define RSILIBRARY_ONLY RayStormBase*/

#define RSILIBRARY
#define RSILIBRARY_ONLY

ULONG ppcRayStormLibOpen()
{
	if(rsiInit(RSILIBRARY_ONLY) != rsiERR_NONE)
		return ERR_RAYSTORMLIB;

	return ERR_NONE;
}

void ppcRayStormLibClose()
{
	if(RayStormBase)
		rsiExit(RSILIBRARY_ONLY);
}

// rsi stub functions

#define STUB_FUNC1(name, arg1) rsiResult stub_ ## name(arg1 a, ULONG *argarray) \
{\
	return name (RSILIBRARY a, rsiTMore, argarray);\
}

#define STUB_FUNC2(name, arg1, arg2) rsiResult stub_ ## name(arg1 b, arg2 a, ULONG *argarray) \
{\
	return name (RSILIBRARY b,a, rsiTMore, argarray);\
}

#define STUB_FUNC5(name, arg1, arg2, arg3, arg4, arg5) rsiResult stub_ ## name(arg1 e, arg2 d, arg3 c, arg4 b, arg5 a, ULONG *argarray) \
{\
	return name (RSILIBRARY e,d,c,b,a, rsiTMore, argarray);\
}

#define STUB_FUNC7(name, arg1, arg2, arg3, arg4, arg5, arg6, arg7) rsiResult stub_ ## name(arg1 g, arg2 f, arg3 e, arg4 d, arg5 c, arg6 b, arg7 a, ULONG *argarray) \
{\
	return name (RSILIBRARY g,f,e,d,c,b,a, rsiTMore, argarray);\
}

STUB_FUNC2(rsiRender, rsiCONTEXT*, UBYTE**)
STUB_FUNC1(rsiSetAntialias, rsiCONTEXT*)
STUB_FUNC1(rsiSetWorld, rsiCONTEXT*)
STUB_FUNC1(rsiSetCamera, rsiCONTEXT*)
STUB_FUNC2(rsiSavePicture, rsiCONTEXT*, char*)
STUB_FUNC2(rsiSetPointLight, rsiCONTEXT*, void*)
STUB_FUNC2(rsiSetSpotLight, rsiCONTEXT*, void*)
STUB_FUNC2(rsiSetDirectionalLight, rsiCONTEXT*, void*)
STUB_FUNC2(rsiSetFlare, rsiCONTEXT*, void*)
STUB_FUNC2(rsiSetStar, rsiCONTEXT*, void*)
STUB_FUNC2(rsiSetSphere, rsiCONTEXT*, void*)
STUB_FUNC2(rsiSetBox, rsiCONTEXT*, void*)
STUB_FUNC2(rsiSetCylinder, rsiCONTEXT*, void*)
STUB_FUNC2(rsiSetSOR, rsiCONTEXT*, void*)
STUB_FUNC2(rsiSetCone, rsiCONTEXT*, void*)
STUB_FUNC5(rsiCreateTriangle, rsiCONTEXT*, void*, rsiVECTOR*, rsiVECTOR*, rsiVECTOR*)
STUB_FUNC7(rsiCreateMesh, rsiCONTEXT*, void*, rsiVECTOR*, rsiEDGE*, int, rsiMESH*, int)
STUB_FUNC2(rsiSetPlane, rsiCONTEXT*, void*)
STUB_FUNC2(rsiSetCSG, rsiCONTEXT*, void*)
STUB_FUNC2(rsiLoadObject, rsiCONTEXT*, char*)
STUB_FUNC2(rsiCreateSurface, rsiCONTEXT*, void**)
STUB_FUNC2(rsiSetSurface, rsiCONTEXT*, void*)
STUB_FUNC2(rsiSetTexture, rsiCONTEXT*, void*)
STUB_FUNC2(rsiSetBrush, rsiCONTEXT*, void*)
STUB_FUNC2(rsiSetHyperTexture, rsiCONTEXT*, void*)
STUB_FUNC2(rsiSetActor, rsiCONTEXT*, void*)
STUB_FUNC5(rsiPosition, rsiCONTEXT*, void*, float, float, rsiVECTOR*)
STUB_FUNC5(rsiAlignment, rsiCONTEXT*, void *, float, float, rsiVECTOR*)
STUB_FUNC5(rsiSize, rsiCONTEXT*, void*, float, float, rsiVECTOR*)

rsiResult stub_rsiSetCooperateCB(rsiCONTEXT*a, void (*b)(void*))
{
	return rsiSetCooperateCB(RSILIBRARY a,b)
}
rsiResult stub_rsiSetCheckCancelCB(rsiCONTEXT*a, BOOL (*b)(void*))
{
	return rsiSetCheckCancelCB(RSILIBRARY a,b)
}
rsiResult stub_rsiSetLogCB(rsiCONTEXT*a, void (*b)(void*, char *))
{
	return rsiSetLogCB(RSILIBRARY a,b)
}
rsiResult stub_rsiSetUpdateStatusCB(rsiCONTEXT*a, void (*b)(void*, float, float, int, int, rsiSMALL_COLOR*))
{
	return rsiSetUpdateStatusCB(RSILIBRARY a,b)
}
rsiResult stub_rsiCreateContext(rsiCONTEXT**a)
{
	return rsiCreateContext(RSILIBRARY a)
}
rsiResult stub_rsiFreeContext(rsiCONTEXT*a)
{
	return rsiFreeContext(RSILIBRARY a)
}
rsiResult stub_rsiSetUserData(rsiCONTEXT*a, void*b)
{
	return rsiSetUserData(RSILIBRARY a,b)
}
rsiResult stub_rsiGetUserData(rsiCONTEXT*a, void**b)
{
	return rsiGetUserData(RSILIBRARY a,b)
}
rsiResult stub_rsiGetErrorMsg(rsiCONTEXT*a, char *b, int c)
{
	return rsiGetErrorMsg(RSILIBRARY a,b,c)
}
rsiResult stub_rsiCleanup(rsiCONTEXT*a)
{
	return rsiCleanup(RSILIBRARY a)
}
/*rsiResult stub_rsiGetStats(rsiSTAT **a, int *b)
{
	return rsiGetStats(RSILIBRARY a,b)
}
rsiResult stub_rsiGetStatDescription(rsiSTAT *a, char *b, int c)
{
	return rsiGetStatDescription(RSILIBRARY a,b,c)
}*/
rsiResult stub_rsiSetOctreeDepth(rsiCONTEXT*a, int b)
{
	return rsiSetOctreeDepth(RSILIBRARY a,b)
}
rsiResult stub_rsiSetRenderField(rsiCONTEXT*a, int b, int c, int d, int e)
{
	return rsiSetRenderField(RSILIBRARY a,b,c,d,e)
}
rsiResult stub_rsiSetDistribution(rsiCONTEXT*a, int b, int c)
{
	return rsiSetDistribution(RSILIBRARY a,b,c)
}
rsiResult stub_rsiSetRandomJitter(rsiCONTEXT*a, BOOL b)
{
	return rsiSetRandomJitter(RSILIBRARY a,b)
}
rsiResult stub_rsiSetScreen(rsiCONTEXT*a, int b, int c)
{
	return rsiSetScreen(RSILIBRARY a,b,c)
}
rsiResult stub_rsiSetBrushPath(rsiCONTEXT*a, char*b)
{
	return rsiSetBrushPath(RSILIBRARY a,b)
}
rsiResult stub_rsiSetTexturePath(rsiCONTEXT*a, char*b)
{
	return rsiSetTexturePath(RSILIBRARY a,b)
}
rsiResult stub_rsiSetObjectPath(rsiCONTEXT*a, char*b)
{
	return rsiSetObjectPath(RSILIBRARY a,b)
}
rsiResult stub_rsiCreatePointLight(rsiCONTEXT*a, void **b)
{
	return rsiCreatePointLight(RSILIBRARY a,b)
}
rsiResult stub_rsiCreateSpotLight(rsiCONTEXT*a, void **b)
{
	return rsiCreateSpotLight(RSILIBRARY a,b)
}
rsiResult stub_rsiCreateDirectionalLight(rsiCONTEXT*a, void **b)
{
	return rsiCreateDirectionalLight(RSILIBRARY a,b)
}
rsiResult stub_rsiCreateFlare(rsiCONTEXT*a, void *b, void *c)
{
	return rsiCreateFlare(RSILIBRARY a,b,c)
}
rsiResult stub_rsiCreateStar(rsiCONTEXT*a, void *b, void *c)
{
	return rsiCreateStar(RSILIBRARY a,b,c)
}
rsiResult stub_rsiCreateSphere(rsiCONTEXT*a, void **b, void *c)
{
	return rsiCreateSphere(RSILIBRARY a,b,c)
}
rsiResult stub_rsiCreateBox(rsiCONTEXT*a, void **b, void *c)
{
	return rsiCreateBox(RSILIBRARY a,b,c)
}
rsiResult stub_rsiCreateCylinder(rsiCONTEXT*a, void **b, void *c)
{
	return rsiCreateCylinder(RSILIBRARY a,b,c)
}
rsiResult stub_rsiCreateSOR(rsiCONTEXT*a, void **b, void *c, int d, rsiVECTOR2D*e)
{
	return rsiCreateSOR(RSILIBRARY a,b,c,d,e)
}
rsiResult stub_rsiCreateCone(rsiCONTEXT*a, void **b, void *c)
{
	return rsiCreateCone(RSILIBRARY a,b,c)
}
rsiResult stub_rsiCreatePlane(rsiCONTEXT*a, void **b, void *c)
{
	return rsiCreatePlane(RSILIBRARY a,b,c)
}
rsiResult stub_rsiCreateCSG(rsiCONTEXT*a, void **b)
{
	return rsiCreateCSG(RSILIBRARY a,b)
}
rsiResult stub_rsiAddImagineTexture(rsiCONTEXT*a, void **b, char *c, void *d)
{
	return rsiAddImagineTexture(RSILIBRARY a,b,c,d)
}
rsiResult stub_rsiAddRayStormTexture(rsiCONTEXT*a, void **b, char *c, void *d)
{
	return rsiAddRayStormTexture(RSILIBRARY a,b,c,d)
}
rsiResult stub_rsiAddBrush(rsiCONTEXT*a, void **b, char *c, void *d)
{
	return rsiAddBrush(RSILIBRARY a,b,c,d)
}
rsiResult stub_rsiAddHyperTexture(rsiCONTEXT*a, void **b, int c, void *d)
{
	return rsiAddHyperTexture(RSILIBRARY a,b,c,d)
}
rsiResult stub_rsiCreateActor(rsiCONTEXT*a, void **b)
{
	return rsiCreateActor(RSILIBRARY a,b)
}

// callback interface (calls 68k functions
void WriteLog(rsiCONTEXT*, char*);
void UpdateStatus(rsiCONTEXT*, float, float, int, int, rsiSMALL_COLOR*);
BOOL CheckCancel(rsiCONTEXT*);
void Cooperate(rsiCONTEXT*);

void __saveds WriteLogInterface(rsiCONTEXT *rc, char *text)
{
	WriteLog(rc, text);
}

void __saveds UpdateStatusInterface(rsiCONTEXT *rc, float percent, float elapsed, int y, int lines, rsiSMALL_COLOR *line)
{
	UpdateStatus(rc, percent, elapsed, y, lines, line);
}

BOOL __saveds CheckCancelInterface(rsiCONTEXT *rc)
{
	return CheckCancel(rc);
}

void __saveds CooperateInterface(rsiCONTEXT *rc)
{
	Cooperate(rc);
}

rsiResult ppcSetCallbacks(rsiCONTEXT *rc)
{
	rsiResult err;

	err = rsiSetLogCB(RSILIBRARY rc, WriteLogInterface);
	if(err)
		return err;
	err = rsiSetUpdateStatusCB(RSILIBRARY rc, UpdateStatusInterface);
	if(err)
		return err;
	err = rsiSetCheckCancelCB(RSILIBRARY rc, CheckCancelInterface);
	if(err)
		return err;
	err = rsiSetCooperateCB(RSILIBRARY rc, CooperateInterface);
	if(err)
		return err;
}

extern "ASM" long ppcSetCallbacks__Pv(void *arg1);
extern "ASM" long __saveds _PPCStub_ppcSetCallbacks__Pv(long *args)
{
	return ppcSetCallbacks__Pv(
		(void *) args[0]
	);
}


// call stubs
#include <libraries/powerpc.h>
#include <pragma/powerpc_lib.h>

extern "ASM" short ppcRayStormLibOpen_();
extern "ASM" short __saveds _PPCStub_ppcRayStormLibOpen_(long *args)
{
	return ppcRayStormLibOpen_(

	);
}
extern "ASM" long stub_rsiSavePicture__PvPcPUj(void *arg1,void *arg2,void*);
extern "ASM" long __saveds _PPCStub_stub_rsiSavePicture__PvPce(long *args)
{
	return stub_rsiSavePicture__PvPcPUj(
		(void *) args[0],
		(void *) args[1],
		(void *) args[2]
	);
}
extern "ASM" long stub_rsiCreateStar__PvPvPv(void *arg1,void *arg2,void *arg3);
extern "ASM" long __saveds _PPCStub_stub_rsiCreateStar__PvPvPv(long *args)
{
	return stub_rsiCreateStar__PvPvPv(
		(void *) args[0],
		(void *) args[1],
		(void *) args[2]
	);
}
extern "ASM" long stub_rsiSetSurface__PvPvPUj(void *arg1,void *arg2,void*);
extern "ASM" long __saveds _PPCStub_stub_rsiSetSurface__PvPve(long *args)
{
	return stub_rsiSetSurface__PvPvPUj(
		(void *) args[0],
		(void *) args[1],
		(void *) args[2]
	);
}
extern "ASM" long stub_rsiSetTexture__PvPvPUj(void *arg1,void *arg2,void*);
extern "ASM" long __saveds _PPCStub_stub_rsiSetTexture__PvPve(long *args)
{
	return stub_rsiSetTexture__PvPvPUj(
		(void *) args[0],
		(void *) args[1],
		(void *) args[2]
	);
}
extern "ASM" long stub_rsiSetBrush__PvPvPUj(void *arg1,void *arg2,void*);
extern "ASM" long __saveds _PPCStub_stub_rsiSetBrush__PvPve(long *args)
{
	return stub_rsiSetBrush__PvPvPUj(
		(void *) args[0],
		(void *) args[1],
		(void *) args[2]
	);
}
extern "ASM" long stub_rsiSetCone__PvPvPUj(void *arg1,void *arg2,void*);
extern "ASM" long __saveds _PPCStub_stub_rsiSetCone__PvPve(long *args)
{
	return stub_rsiSetCone__PvPvPUj(
		(void *) args[0],
		(void *) args[1],
		(void *) args[2]
	);
}
extern "ASM" long stub_rsiSetFlare__PvPvPUj(void *arg1,void *arg2,void*);
extern "ASM" long __saveds _PPCStub_stub_rsiSetFlare__PvPve(long *args)
{
	return stub_rsiSetFlare__PvPvPUj(
		(void *) args[0],
		(void *) args[1],
		(void *) args[2]
	);
}
extern "ASM" long stub_rsiSetSpotLight__PvPvPUj(void *arg1,void *arg2,void*);
extern "ASM" long __saveds _PPCStub_stub_rsiSetSpotLight__PvPve(long *args)
{
	return stub_rsiSetSpotLight__PvPvPUj(
		(void *) args[0],
		(void *) args[1],
		(void *) args[2]
	);
}
extern "ASM" long stub_rsiSetPlane__PvPvPUj(void *arg1,void *arg2,void*);
extern "ASM" long __saveds _PPCStub_stub_rsiSetPlane__PvPve(long *args)
{
	return stub_rsiSetPlane__PvPvPUj(
		(void *) args[0],
		(void *) args[1],
		(void *) args[2]
	);
}
extern "ASM" long stub_rsiSetSOR__PvPvPUj(void *arg1,void *arg2,void*);
extern "ASM" long __saveds _PPCStub_stub_rsiSetSOR__PvPve(long *args)
{
	return stub_rsiSetSOR__PvPvPUj(
		(void *) args[0],
		(void *) args[1],
		(void *) args[2]
	);
}
extern "ASM" long stub_rsiCreateSphere__PvPPvPv(void *arg1,void *arg2,void *arg3);
extern "ASM" long __saveds _PPCStub_stub_rsiCreateSphere__PvPPvPv(long *args)
{
	return stub_rsiCreateSphere__PvPPvPv(
		(void *) args[0],
		(void *) args[1],
		(void *) args[2]
	);
}
extern "ASM" long stub_rsiCreateBox__PvPPvPv(void *arg1,void *arg2,void *arg3);
extern "ASM" long __saveds _PPCStub_stub_rsiCreateBox__PvPPvPv(long *args)
{
	return stub_rsiCreateBox__PvPPvPv(
		(void *) args[0],
		(void *) args[1],
		(void *) args[2]
	);
}
extern "ASM" long stub_rsiSetCamera__PvPUj(void *arg1,void*);
extern "ASM" long __saveds _PPCStub_stub_rsiSetCamera__Pve(long *args)
{
	return stub_rsiSetCamera__PvPUj(
		(void *) args[0],
		(void *) args[1]
	);
}
extern "ASM" long stub_rsiSetCSG__PvPvPUj(void *arg1,void *arg2,void*);
extern "ASM" long __saveds _PPCStub_stub_rsiSetCSG__PvPve(long *args)
{
	return stub_rsiSetCSG__PvPvPUj(
		(void *) args[0],
		(void *) args[1],
		(void *) args[2]
	);
}
extern "ASM" long stub_rsiCreateCylinder__PvPPvPv(void *arg1,void *arg2,void *arg3);
extern "ASM" long __saveds _PPCStub_stub_rsiCreateCylinder__PvPPvPv(long *args)
{
	return stub_rsiCreateCylinder__PvPPvPv(
		(void *) args[0],
		(void *) args[1],
		(void *) args[2]
	);
}
extern "ASM" void ppcRayStormLibClose_();
extern "ASM" void __saveds _PPCStub_ppcRayStormLibClose_(long *args)
{
	ppcRayStormLibClose_(

	);
}
extern "ASM" long stub_rsiCleanup__Pv(void *arg1);
extern "ASM" long __saveds _PPCStub_stub_rsiCleanup__Pv(long *args)
{
	return stub_rsiCleanup__Pv(
		(void *) args[0]
	);
}
extern "ASM" long stub_rsiCreateSOR__PvPPvPviP11rsiVECTOR2D(void *arg1,void *arg2,void *arg3,long arg4,void *arg5);
extern "ASM" long __saveds _PPCStub_stub_rsiCreateSOR__PvPPvPviP11rsiVECTOR2D(long *args)
{
	return stub_rsiCreateSOR__PvPPvPviP11rsiVECTOR2D(
		(void *) args[0],
		(void *) args[1],
		(void *) args[2],
		(long) args[3],
		(void *) args[4]
	);
}
extern "ASM" long stub_rsiSetSphere__PvPvPUj(void *arg1,void *arg2,void*);
extern "ASM" long __saveds _PPCStub_stub_rsiSetSphere__PvPve(long *args)
{
	return stub_rsiSetSphere__PvPvPUj(
		(void *) args[0],
		(void *) args[1],
		(void *) args[2]
	);
}
extern "ASM" long stub_rsiSetStar__PvPvPUj(void *arg1,void *arg2,void*);
extern "ASM" long __saveds _PPCStub_stub_rsiSetStar__PvPve(long *args)
{
	return stub_rsiSetStar__PvPvPUj(
		(void *) args[0],
		(void *) args[1],
		(void *) args[2]
	);
}
extern "ASM" long stub_rsiCreateSurface__PvPPvPUj(void *arg1,void *arg2,void*);
extern "ASM" long __saveds _PPCStub_stub_rsiCreateSurface__PvPPve(long *args)
{
	return stub_rsiCreateSurface__PvPPvPUj(
		(void *) args[0],
		(void *) args[1],
		(void *) args[2]
	);
}
extern "ASM" long stub_rsiAddImagineTexture__PvPPvPcPv(void *arg1,void *arg2,void *arg3,void *arg4);
extern "ASM" long __saveds _PPCStub_stub_rsiAddImagineTexture__PvPPvPcPv(long *args)
{
	return stub_rsiAddImagineTexture__PvPPvPcPv(
		(void *) args[0],
		(void *) args[1],
		(void *) args[2],
		(void *) args[3]
	);
}
extern "ASM" long stub_rsiAddRayStormTexture__PvPPvPcPv(void *arg1,void *arg2,void *arg3,void *arg4);
extern "ASM" long __saveds _PPCStub_stub_rsiAddRayStormTexture__PvPPvPcPv(long *args)
{
	return stub_rsiAddRayStormTexture__PvPPvPcPv(
		(void *) args[0],
		(void *) args[1],
		(void *) args[2],
		(void *) args[3]
	);
}
extern "ASM" long stub_rsiSetBox__PvPvPUj(void *arg1,void *arg2,void*);
extern "ASM" long __saveds _PPCStub_stub_rsiSetBox__PvPve(long *args)
{
	return stub_rsiSetBox__PvPvPUj(
		(void *) args[0],
		(void *) args[1],
		(void *) args[2]
	);
}
extern "ASM" long stub_rsiAddBrush__PvPPvPcPv(void *arg1,void *arg2,void *arg3,void *arg4);
extern "ASM" long __saveds _PPCStub_stub_rsiAddBrush__PvPPvPcPv(long *args)
{
	return stub_rsiAddBrush__PvPPvPcPv(
		(void *) args[0],
		(void *) args[1],
		(void *) args[2],
		(void *) args[3]
	);
}
extern "ASM" long stub_rsiCreateCone__PvPPvPv(void *arg1,void *arg2,void *arg3);
extern "ASM" long __saveds _PPCStub_stub_rsiCreateCone__PvPPvPv(long *args)
{
	return stub_rsiCreateCone__PvPPvPv(
		(void *) args[0],
		(void *) args[1],
		(void *) args[2]
	);
}
extern "ASM" long stub_rsiCreateCSG__PvPPv(void *arg1,void *arg2);
extern "ASM" long __saveds _PPCStub_stub_rsiCreateCSG__PvPPv(long *args)
{
	return stub_rsiCreateCSG__PvPPv(
		(void *) args[0],
		(void *) args[1]
	);
}
extern "ASM" long stub_rsiSetCylinder__PvPvPUj(void *arg1,void *arg2,void*);
extern "ASM" long __saveds _PPCStub_stub_rsiSetCylinder__PvPve(long *args)
{
	return stub_rsiSetCylinder__PvPvPUj(
		(void *) args[0],
		(void *) args[1],
		(void *) args[2]
	);
}
extern "ASM" long stub_rsiCreateFlare__PvPvPv(void *arg1,void *arg2,void *arg3);
extern "ASM" long __saveds _PPCStub_stub_rsiCreateFlare__PvPvPv(long *args)
{
	return stub_rsiCreateFlare__PvPvPv(
		(void *) args[0],
		(void *) args[1],
		(void *) args[2]
	);
}
extern "ASM" long stub_rsiCreatePointLight__PvPPv(void *arg1,void *arg2);
extern "ASM" long __saveds _PPCStub_stub_rsiCreatePointLight__PvPPv(long *args)
{
	return stub_rsiCreatePointLight__PvPPv(
		(void *) args[0],
		(void *) args[1]
	);
}
extern "ASM" long stub_rsiSetLogCB__PvPFvPvPcp(void *arg1,void *arg2);
extern "ASM" long __saveds _PPCStub_stub_rsiSetLogCB__PvPFvPvPcp(long *args)
{
	return stub_rsiSetLogCB__PvPFvPvPcp(
		(void *) args[0],
		(void *) args[1]
	);
}
extern "ASM" long stub_rsiGetErrorMsg__PvPci(void *arg1,void *arg2,long arg3);
extern "ASM" long __saveds _PPCStub_stub_rsiGetErrorMsg__PvPci(long *args)
{
	return stub_rsiGetErrorMsg__PvPci(
		(void *) args[0],
		(void *) args[1],
		(long) args[2]
	);
}
extern "ASM" long stub_rsiSetPointLight__PvPvPUj(void *arg1,void *arg2,void*);
extern "ASM" long __saveds _PPCStub_stub_rsiSetPointLight__PvPve(long *args)
{
	return stub_rsiSetPointLight__PvPvPUj(
		(void *) args[0],
		(void *) args[1],
		(void *) args[2]
	);
}
extern "ASM" long stub_rsiCreatePlane__PvPPvPv(void *arg1,void *arg2,void *arg3);
extern "ASM" long __saveds _PPCStub_stub_rsiCreatePlane__PvPPvPv(long *args)
{
	return stub_rsiCreatePlane__PvPPvPv(
		(void *) args[0],
		(void *) args[1],
		(void *) args[2]
	);
}
extern "ASM" long stub_rsiSetCooperateCB__PvPFvPvp(void *arg1,void *arg2);
extern "ASM" long __saveds _PPCStub_stub_rsiSetCooperateCB__PvPFvPvp(long *args)
{
	return stub_rsiSetCooperateCB__PvPFvPvp(
		(void *) args[0],
		(void *) args[1]
	);
}
extern "ASM" long stub_rsiCreateSpotLight__PvPPv(void *arg1,void *arg2);
extern "ASM" long __saveds _PPCStub_stub_rsiCreateSpotLight__PvPPv(long *args)
{
	return stub_rsiCreateSpotLight__PvPPv(
		(void *) args[0],
		(void *) args[1]
	);
}
extern "ASM" long stub_rsiSetUpdateStatusCB__PvPFvPvffiiP14rsiSMALL_COLORp(void *arg1,void *arg2);
extern "ASM" long __saveds _PPCStub_stub_rsiSetUpdateStatusCB__PvPFvPvffiiP14rsiSMALL_COLORp(long *args)
{
	return stub_rsiSetUpdateStatusCB__PvPFvPvffiiP14rsiSMALL_COLORp(
		(void *) args[0],
		(void *) args[1]
	);
}
extern "ASM" long stub_rsiSetUserData__PvPv(void *arg1,void *arg2);
extern "ASM" long __saveds _PPCStub_stub_rsiSetUserData__PvPv(long *args)
{
	return stub_rsiSetUserData__PvPv(
		(void *) args[0],
		(void *) args[1]
	);
}
extern "ASM" long stub_rsiCreateContext__PPv(void *arg1);
extern "ASM" long __saveds _PPCStub_stub_rsiCreateContext__PPv(long *args)
{
	return stub_rsiCreateContext__PPv(
		(void *) args[0]
	);
}
extern "ASM" long stub_rsiFreeContext__Pv(void *arg1);
extern "ASM" long __saveds _PPCStub_stub_rsiFreeContext__Pv(long *args)
{
	return stub_rsiFreeContext__Pv(
		(void *) args[0]
	);
}
extern "ASM" long stub_rsiSetCheckCancelCB__PvPFsPvp(void *arg1,void *arg2);
extern "ASM" long __saveds _PPCStub_stub_rsiSetCheckCancelCB__PvPFsPvp(long *args)
{
	return stub_rsiSetCheckCancelCB__PvPFsPvp(
		(void *) args[0],
		(void *) args[1]
	);
}
extern "ASM" long stub_rsiLoadObject__PvPcPUj(void *arg1,void *arg2,void*);
extern "ASM" long __saveds _PPCStub_stub_rsiLoadObject__PvPce(long *args)
{
	return stub_rsiLoadObject__PvPcPUj(
		(void *) args[0],
		(void *) args[1],
		(void *) args[2]
	);
}
extern "ASM" void _68kStub_Cooperate__Pv(void);
extern "ASM" void Cooperate__Pv(void *arg1)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_Cooperate__Pv)(void) = _68kStub_Cooperate__Pv;
	ppc.PP_Code = _68kData_Cooperate__Pv;
	long args[1];
	args[0] = (long) arg1;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
}
extern "ASM" void _68kStub_CheckCancel__Pv(void);
extern "ASM" short CheckCancel__Pv(void *arg1)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_CheckCancel__Pv)(void) = _68kStub_CheckCancel__Pv;
	ppc.PP_Code = _68kData_CheckCancel__Pv;
	long args[1];
	args[0] = (long) arg1;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
	return (short) ppc.PP_Regs[0];
}
extern "ASM" void _68kStub_UpdateStatus__PvffiiP14rsiSMALL_COLOR(void);
extern "ASM" void UpdateStatus__PvffiiP14rsiSMALL_COLOR(void *arg1,float arg2,float arg3,long arg4,long arg5,void *arg6)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_UpdateStatus__PvffiiP14rsiSMALL_COLOR)(void) = _68kStub_UpdateStatus__PvffiiP14rsiSMALL_COLOR;
	ppc.PP_Code = _68kData_UpdateStatus__PvffiiP14rsiSMALL_COLOR;
	long args[6];
	args[0] = (long) arg1;
	*((float *) &args[1]) = arg2;
	*((float *) &args[2]) = arg3;
	args[3] = (long) arg4;
	args[4] = (long) arg5;
	args[5] = (long) arg6;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
}
extern "ASM" void _68kStub_WriteLog__PvPc(void);
extern "ASM" void WriteLog__PvPc(void *arg1,void *arg2)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_WriteLog__PvPc)(void) = _68kStub_WriteLog__PvPc;
	ppc.PP_Code = _68kData_WriteLog__PvPc;
	long args[2];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
}
extern "ASM" long stub_rsiGetUserData__PvPPv(void *arg1,void *arg2);
extern "ASM" long __saveds _PPCStub_stub_rsiGetUserData__PvPPv(long *args)
{
	return stub_rsiGetUserData__PvPPv(
		(void *) args[0],
		(void *) args[1]
	);
}
extern "ASM" long stub_rsiSetObjectPath__PvPc(void *arg1,void *arg2);
extern "ASM" long __saveds _PPCStub_stub_rsiSetObjectPath__PvPc(long *args)
{
	return stub_rsiSetObjectPath__PvPc(
		(void *) args[0],
		(void *) args[1]
	);
}
extern "ASM" long stub_rsiSetBrushPath__PvPc(void *arg1,void *arg2);
extern "ASM" long __saveds _PPCStub_stub_rsiSetBrushPath__PvPc(long *args)
{
	return stub_rsiSetBrushPath__PvPc(
		(void *) args[0],
		(void *) args[1]
	);
}
extern "ASM" long stub_rsiSetRenderField__Pviiii(void *arg1,long arg2,long arg3,long arg4,long arg5);
extern "ASM" long __saveds _PPCStub_stub_rsiSetRenderField__Pviiii(long *args)
{
	return stub_rsiSetRenderField__Pviiii(
		(void *) args[0],
		(long) args[1],
		(long) args[2],
		(long) args[3],
		(long) args[4]
	);
}
extern "ASM" long stub_rsiSetTexturePath__PvPc(void *arg1,void *arg2);
extern "ASM" long __saveds _PPCStub_stub_rsiSetTexturePath__PvPc(long *args)
{
	return stub_rsiSetTexturePath__PvPc(
		(void *) args[0],
		(void *) args[1]
	);
}
extern "ASM" long stub_rsiRender__PvPPUcPUj(void *arg1,void *arg2,void*);
extern "ASM" long __saveds _PPCStub_stub_rsiRender__PvPPUce(long *args)
{
	return stub_rsiRender__PvPPUcPUj(
		(void *) args[0],
		(void *) args[1],
		(void *) args[2]
	);
}
extern "ASM" long stub_rsiSetAntialias__PvPUj(void *arg1,void*);
extern "ASM" long __saveds _PPCStub_stub_rsiSetAntialias__Pve(long *args)
{
	return stub_rsiSetAntialias__PvPUj(
		(void *) args[0],
		(void *) args[1]
	);
}
extern "ASM" long stub_rsiSetWorld__PvPUj(void *arg1,void*);
extern "ASM" long __saveds _PPCStub_stub_rsiSetWorld__Pve(long *args)
{
	return stub_rsiSetWorld__PvPUj(
		(void *) args[0],
		(void *) args[1]
	);
}
extern "ASM" long stub_rsiSetDistribution__Pvii(void *arg1,long arg2,long arg3);
extern "ASM" long __saveds _PPCStub_stub_rsiSetDistribution__Pvii(long *args)
{
	return stub_rsiSetDistribution__Pvii(
		(void *) args[0],
		(long) args[1],
		(long) args[2]
	);
}
extern "ASM" long stub_rsiSetScreen__Pvii(void *arg1,long arg2,long arg3);
extern "ASM" long __saveds _PPCStub_stub_rsiSetScreen__Pvii(long *args)
{
	return stub_rsiSetScreen__Pvii(
		(void *) args[0],
		(long) args[1],
		(long) args[2]
	);
}
extern "ASM" long stub_rsiSetOctreeDepth__Pvi(void *arg1,long arg2);
extern "ASM" long __saveds _PPCStub_stub_rsiSetOctreeDepth__Pvi(long *args)
{
	return stub_rsiSetOctreeDepth__Pvi(
		(void *) args[0],
		(long) args[1]
	);
}
extern "ASM" long stub_rsiCreateMesh__PvPvP09rsiVECTORP07rsiEDGEiP07rsiMESHiPUj(void *arg1,void *arg2,void *arg3,void *arg4,long arg5,void *arg6,long arg7, void*);
extern "ASM" long __saveds _PPCStub_stub_rsiCreateMesh__PvPvP09rsiVECTORP07rsiEDGEiP07rsiMESHie(long *args)
{
	return stub_rsiCreateMesh__PvPvP09rsiVECTORP07rsiEDGEiP07rsiMESHiPUj(
		(void *) args[0],
		(void *) args[1],
		(void *) args[2],
		(void *) args[3],
		(long) args[4],
		(void *) args[5],
		(long) args[6],
		(void *) args[7]
	);
}

extern "ASM" long stub_rsiCreateDirectionalLight__PvPPv(void *arg1,void *arg2);
extern "ASM" long __saveds _PPCStub_stub_rsiCreateDirectionalLight__PvPPv(long *args)
{
	return stub_rsiCreateDirectionalLight__PvPPv(
		(void *) args[0],
		(void *) args[1]
	);
}

extern "ASM" long stub_rsiSetDirectionalLight__PvPvPUj(void *arg1,void *arg2,void*);
extern "ASM" long __saveds _PPCStub_stub_rsiSetDirectionalLight__PvPve(long *args)
{
	return stub_rsiSetDirectionalLight__PvPvPUj(
		(void *) args[0],
		(void *) args[1],
		(void *) args[2]
	);
}
