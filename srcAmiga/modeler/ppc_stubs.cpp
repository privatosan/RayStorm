#include <libraries/powerpc.h>
#include <pragma/powerpc_lib.h>
#include <intuition/classusr.h>
#include <clib/exec_protos.h>
#include <stdarg.h>
#include <string.h>
#include "preview.h"

extern ULONG DoMethodAPPC(Object *obj, Msg msg);
extern ULONG DoSuperMethodAPPC(struct IClass *cl, Object *obj, Msg message);
extern APTR MUI_NewObjectAPPC(char *classID, struct TagItem *tags);

extern struct Library *PowerPCBase;
struct Library *TextureBase;

// stub functions

extern "ASM" void _68kStub_DoMethodAPPC__PUjP00(void);
extern "ASM" unsigned long DoMethodAPPC__PUjP00(void *arg1,void *arg2)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_DoMethodAPPC__PUjP00)(void) = _68kStub_DoMethodAPPC__PUjP00;
	ppc.PP_Code = _68kData_DoMethodAPPC__PUjP00;
	long args[2];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
	return (unsigned long) ppc.PP_Regs[0];
}

ULONG DoMethodPPC(Object *obj, Tag tag, ...)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_DoMethodAPPC__PUjP00)(void) = _68kStub_DoMethodAPPC__PUjP00;
	ppc.PP_Code = _68kData_DoMethodAPPC__PUjP00;
	long args[2];
	args[0] = (long) obj;
	args[1] = (long) &tag;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
	return (unsigned long) ppc.PP_Regs[0];
}

extern "ASM" void _68kStub_DoSuperMethodAPPC__P06IClassPUjP00(void);
extern "ASM" unsigned long DoSuperMethodAPPC__P06IClassPUjP00(void *arg1,void *arg2,void *arg3)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_DoSuperMethodAPPC__P06IClassPUjP00)(void) = _68kStub_DoSuperMethodAPPC__P06IClassPUjP00;
	ppc.PP_Code = _68kData_DoSuperMethodAPPC__P06IClassPUjP00;
	long args[3];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) arg3;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
	return (unsigned long) ppc.PP_Regs[0];
}

ULONG DoSuperNew(struct IClass *cl, Object *obj, Tag tag, ...)
{
	struct opSet new_message =
	{
		OM_NEW,
		(struct TagItem*)&tag,
		NULL
	};

	return DoSuperMethodAPPC(cl, obj, (Msg)&new_message);
}

extern "ASM" void _68kStub_MUI_NewObjectAPPC__PcP07TagItem(void);
extern "ASM" void *MUI_NewObjectAPPC__PcP07TagItem(void *arg1,void *arg2)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_MUI_NewObjectAPPC__PcP07TagItem)(void) = _68kStub_MUI_NewObjectAPPC__PcP07TagItem;
	ppc.PP_Code = _68kData_MUI_NewObjectAPPC__PcP07TagItem;
	long args[2];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
	return (void *) ppc.PP_Regs[0];
}

extern "C" Object *MUI_NewObject(char *classID, Tag tag, ...)
{
	return (Object*)MUI_NewObjectAPPC(classID, (struct TagItem*)&tag);
}

extern "ASM" void _68kStub_MUI_MakeObjectAPPC__UjPUj(void);
extern "ASM" void *MUI_MakeObjectAPPC__UjPUj(unsigned long arg1,void *arg2)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_MUI_MakeObjectAPPC__UjPUj)(void) = _68kStub_MUI_MakeObjectAPPC__UjPUj;
	ppc.PP_Code = _68kData_MUI_MakeObjectAPPC__UjPUj;
	long args[2];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
	return (void *) ppc.PP_Regs[0];
}

extern Object* MUI_MakeObjectAPPC(ULONG type, ULONG* tag);
extern "C" Object *MUI_MakeObject(ULONG type, ULONG tag, ...)
{
	return MUI_MakeObjectAPPC(type, (ULONG*)&tag);
}

//---------------------------------------------------
// RayStorm texture functions

#include "rtxt:rtexture.h"
#include "rtxt:texture_module.h"

extern "ASM" void __saveds _PPCStub__PPCRTextureWork(long *args)
{
	((TEXTURE_INFO*)args[0])->work((TEXTURE_INFO*)args[0], (void*)args[1], (TEXTURE_PATCH*)args[2], (VECTOR*)args[3]);
}

TEXTURE_INFO *stub_texture_init(char *name, struct Library **Base)
{
	*Base = OpenLibrary(name, 0);
	if(!Base)
		return NULL;
	TextureBase = *Base;

	return texture_init();
}

extern "ASM" void *stub_texture_init__PcPP07Library(void *arg1,void *arg2);
extern "ASM" void *__saveds _PPCStub_stub_texture_init__PcPP07Library(long *args)
{
	return stub_texture_init__PcPP07Library(
		(void *) args[0],
		(void *) args[1]
	);
}

void PPCRTextureCleanup(void (*cleanup)(_tinfo *tinfo), TEXTURE_INFO *tinfo, struct Library *Base)
{
	cleanup(tinfo);
	CloseLibrary(Base);
}

extern "ASM" void PPCRTextureCleanup__PFvP06_tinfopP06_tinfoP07Library(void *arg1,void *arg2,void *arg3);
extern "ASM" void __saveds _PPCStub_PPCRTextureCleanup__PFvP06_tinfopP06_tinfoP07Library(long *args)
{
	PPCRTextureCleanup__PFvP06_tinfopP06_tinfoP07Library(
		(void *) args[0],
		(void *) args[1],
		(void *) args[2]
	);
}

//---------------------------------------------------
// Preview functions

extern "ASM" void _68kStub_ITextureWork__P09IM_TTABLEPfP08IM_PATCHP06VECTORPf(void);
extern "ASM" void ITextureWork__P09IM_TTABLEPfP08IM_PATCHP06VECTORPf(void *arg1,void *arg2,void *arg3,void *arg4,void *arg5)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_ITextureWork__P09IM_TTABLEPfP08IM_PATCHP06VECTORPf)(void) = _68kStub_ITextureWork__P09IM_TTABLEPfP08IM_PATCHP06VECTORPf;
	ppc.PP_Code = _68kData_ITextureWork__P09IM_TTABLEPfP08IM_PATCHP06VECTORPf;
	long args[5];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) arg3;
	args[3] = (long) arg4;
	args[4] = (long) arg5;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
}

extern "ASM" void _68kStub_ITextureInit__PFP09IM_TTABLEjp(void);
extern "ASM" void *ITextureInit__PFP09IM_TTABLEjp(void *arg1)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_ITextureInit__PFP09IM_TTABLEjp)(void) = _68kStub_ITextureInit__PFP09IM_TTABLEjp;
	ppc.PP_Code = _68kData_ITextureInit__PFP09IM_TTABLEjp;
	long args[1];
	args[0] = (long) arg1;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
	return (void *) ppc.PP_Regs[0];
}

extern "ASM" void _68kStub_ITextureCleanup__P09IM_TTABLE(void);
extern "ASM" void ITextureCleanup__P09IM_TTABLE(void *arg1)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_ITextureCleanup__P09IM_TTABLE)(void) = _68kStub_ITextureCleanup__P09IM_TTABLE;
	ppc.PP_Code = _68kData_ITextureCleanup__P09IM_TTABLE;
	long args[1];
	args[0] = (long) arg1;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
}

extern "ASM" void _68kStub_GetNext__P07TEXTURE(void);
extern "ASM" void *GetNext__P07TEXTURE(void *arg1)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_GetNext__P07TEXTURE)(void) = _68kStub_GetNext__P07TEXTURE;
	ppc.PP_Code = _68kData_GetNext__P07TEXTURE;
	long args[1];
	args[0] = (long) arg1;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
	return (void *) ppc.PP_Regs[0];
}

extern "ASM" void _68kStub_GetNext__P14TEXTURE_OBJECT(void);
extern "ASM" void *GetNext__P14TEXTURE_OBJECT(void *arg1)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_GetNext__P14TEXTURE_OBJECT)(void) = _68kStub_GetNext__P14TEXTURE_OBJECT;
	ppc.PP_Code = _68kData_GetNext__P14TEXTURE_OBJECT;
	long args[1];
	args[0] = (long) arg1;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
	return (void *) ppc.PP_Regs[0];
}

extern "ASM" void _68kStub_GetType__P07TEXTURE(void);
extern "ASM" long GetType__P07TEXTURE(void *arg1)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_GetType__P07TEXTURE)(void) = _68kStub_GetType__P07TEXTURE;
	ppc.PP_Code = _68kData_GetType__P07TEXTURE;
	long args[1];
	args[0] = (long) arg1;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
	return (long) ppc.PP_Regs[0];
}

extern "ASM" void *__saveds _PPCStub_PreviewCreate_(long *args)
{
	return new PREVIEW;
}

extern "ASM" short __saveds _PPCStub_Init__P07PREVIEWiiiP07SURFACEP06OBJECT(long *args)
{
	return ((PREVIEW*)args[0])->Init(
		(int) args[1],
		(int) args[2],
		(int) args[3],
		(SURFACE *) args[4],
		(OBJECT *) args[5]
	);
}

extern "ASM" void __saveds _PPCStub_RenderLines__P07PREVIEWi(long *args)
{
	((PREVIEW*)args[0])->RenderLines(
		(int) args[1]
	);
}

extern "ASM" void *__saveds _PPCStub_GetLine__P07PREVIEW(long *args)
{
	return ((PREVIEW*)args[0])->GetLine();
}

extern "ASM" void __saveds _PPCStub_PreviewDelete__P07PREVIEW(long *args)
{
	delete (PREVIEW*)args[0];
}



