#include <libraries/powerpc.h>
#include <pragma/powerpc_lib.h>

extern struct Library *PowerPCBase;
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

extern "ASM" unsigned long size__PUj(void *arg1);
extern "ASM" unsigned long __saveds _PPCStub_size__PUj(long *args)
{
	return size__PUj(
		(void *) args[0]
	);
}

extern "ASM" unsigned long transluc__PUj(void *arg1);
extern "ASM" unsigned long __saveds _PPCStub_transluc__PUj(long *args)
{
	return transluc__PUj(
		(void *) args[0]
	);
}

extern "ASM" unsigned long hypertexture__PUj(void *arg1);
extern "ASM" unsigned long __saveds _PPCStub_hypertexture__PUj(long *args)
{
	return hypertexture__PUj(
		(void *) args[0]
	);
}

extern "ASM" void *InitInterface_();
extern "ASM" void *__saveds _PPCStub_InitInterface_(long *args)
{
	return InitInterface_(

	);
}

extern "ASM" unsigned long antialias__PUj(void *arg1);
extern "ASM" unsigned long __saveds _PPCStub_antialias__PUj(long *args)
{
	return antialias__PUj(
		(void *) args[0]
	);
}

extern "ASM" unsigned long cone__PUj(void *arg1);
extern "ASM" unsigned long __saveds _PPCStub_cone__PUj(long *args)
{
	return cone__PUj(
		(void *) args[0]
	);
}

extern "ASM" unsigned long setworld__PUj(void *arg1);
extern "ASM" unsigned long __saveds _PPCStub_setworld__PUj(long *args)
{
	return setworld__PUj(
		(void *) args[0]
	);
}

extern "ASM" unsigned long cleanup__PUj(void *arg1);
extern "ASM" unsigned long __saveds _PPCStub_cleanup__PUj(long *args)
{
	return cleanup__PUj(
		(void *) args[0]
	);
}

extern "ASM" unsigned long box__PUj(void *arg1);
extern "ASM" unsigned long __saveds _PPCStub_box__PUj(long *args)
{
	return box__PUj(
		(void *) args[0]
	);
}

extern "ASM" unsigned long diffuse__PUj(void *arg1);
extern "ASM" unsigned long __saveds _PPCStub_diffuse__PUj(long *args)
{
	return diffuse__PUj(
		(void *) args[0]
	);
}

extern "ASM" unsigned long difftrans__PUj(void *arg1);
extern "ASM" unsigned long __saveds _PPCStub_difftrans__PUj(long *args)
{
	return difftrans__PUj(
		(void *) args[0]
	);
}

extern "ASM" unsigned long transpar__PUj(void *arg1);
extern "ASM" unsigned long __saveds _PPCStub_transpar__PUj(long *args)
{
	return transpar__PUj(
		(void *) args[0]
	);
}

extern "ASM" unsigned long texturepath__PUj(void *arg1);
extern "ASM" unsigned long __saveds _PPCStub_texturepath__PUj(long *args)
{
	return texturepath__PUj(
		(void *) args[0]
	);
}

extern "ASM" unsigned long pointlight__PUj(void *arg1);
extern "ASM" unsigned long __saveds _PPCStub_pointlight__PUj(long *args)
{
	return pointlight__PUj(
		(void *) args[0]
	);
}

extern "ASM" unsigned long spotlight__PUj(void *arg1);
extern "ASM" unsigned long __saveds _PPCStub_spotlight__PUj(long *args)
{
	return spotlight__PUj(
		(void *) args[0]
	);
}

extern "ASM" unsigned long directionallight__PUj(void *arg1);
extern "ASM" unsigned long __saveds _PPCStub_directionallight__PUj(long *args)
{
	return directionallight__PUj(
		(void *) args[0]
	);
}

extern "ASM" unsigned long cylinder__PUj(void *arg1);
extern "ASM" unsigned long __saveds _PPCStub_cylinder__PUj(long *args)
{
	return cylinder__PUj(
		(void *) args[0]
	);
}

extern "ASM" unsigned long triangle__PUj(void *arg1);
extern "ASM" unsigned long __saveds _PPCStub_triangle__PUj(long *args)
{
	return triangle__PUj(
		(void *) args[0]
	);
}

extern "ASM" unsigned long brush__PUj(void *arg1);
extern "ASM" unsigned long __saveds _PPCStub_brush__PUj(long *args)
{
	return brush__PUj(
		(void *) args[0]
	);
}

extern "ASM" unsigned long transexp__PUj(void *arg1);
extern "ASM" unsigned long __saveds _PPCStub_transexp__PUj(long *args)
{
	return transexp__PUj(
		(void *) args[0]
	);
}

extern "ASM" unsigned long star__PUj(void *arg1);
extern "ASM" unsigned long __saveds _PPCStub_star__PUj(long *args)
{
	return star__PUj(
		(void *) args[0]
	);
}

extern "ASM" unsigned long imtexture__PUj(void *arg1);
extern "ASM" unsigned long __saveds _PPCStub_imtexture__PUj(long *args)
{
	return imtexture__PUj(
		(void *) args[0]
	);
}

extern "ASM" unsigned long reflect__PUj(void *arg1);
extern "ASM" unsigned long __saveds _PPCStub_reflect__PUj(long *args)
{
	return reflect__PUj(
		(void *) args[0]
	);
}

extern "ASM" unsigned long startrender__PUj(void *arg1);
extern "ASM" unsigned long __saveds _PPCStub_startrender__PUj(long *args)
{
	return startrender__PUj(
		(void *) args[0]
	);
}

extern "ASM" unsigned long setscreen__PUj(void *arg1);
extern "ASM" unsigned long __saveds _PPCStub_setscreen__PUj(long *args)
{
	return setscreen__PUj(
		(void *) args[0]
	);
}

extern "ASM" unsigned long refexp__PUj(void *arg1);
extern "ASM" unsigned long __saveds _PPCStub_refexp__PUj(long *args)
{
	return refexp__PUj(
		(void *) args[0]
	);
}

extern "ASM" unsigned long sphere__PUj(void *arg1);
extern "ASM" unsigned long __saveds _PPCStub_sphere__PUj(long *args)
{
	return sphere__PUj(
		(void *) args[0]
	);
}

extern "ASM" unsigned long newsurface__PUj(void *arg1);
extern "ASM" unsigned long __saveds _PPCStub_newsurface__PUj(long *args)
{
	return newsurface__PUj(
		(void *) args[0]
	);
}

extern "ASM" unsigned long specular__PUj(void *arg1);
extern "ASM" unsigned long __saveds _PPCStub_specular__PUj(long *args)
{
	return specular__PUj(
		(void *) args[0]
	);
}

extern "ASM" unsigned long position__PUj(void *arg1);
extern "ASM" unsigned long __saveds _PPCStub_position__PUj(long *args)
{
	return position__PUj(
		(void *) args[0]
	);
}

extern "ASM" unsigned long ambient__PUj(void *arg1);
extern "ASM" unsigned long __saveds _PPCStub_ambient__PUj(long *args)
{
	return ambient__PUj(
		(void *) args[0]
	);
}

extern "ASM" unsigned long distrib__PUj(void *arg1);
extern "ASM" unsigned long __saveds _PPCStub_distrib__PUj(long *args)
{
	return distrib__PUj(
		(void *) args[0]
	);
}

extern "ASM" unsigned long setcamera__PUj(void *arg1);
extern "ASM" unsigned long __saveds _PPCStub_setcamera__PUj(long *args)
{
	return setcamera__PUj(
		(void *) args[0]
	);
}

extern "ASM" unsigned long refrindex__PUj(void *arg1);
extern "ASM" unsigned long __saveds _PPCStub_refrindex__PUj(long *args)
{
	return refrindex__PUj(
		(void *) args[0]
	);
}

extern "ASM" unsigned long flare__PUj(void *arg1);
extern "ASM" unsigned long __saveds _PPCStub_flare__PUj(long *args)
{
	return flare__PUj(
		(void *) args[0]
	);
}

extern "ASM" void CleanupInterface_();
extern "ASM" void __saveds _PPCStub_CleanupInterface_(long *args)
{
	CleanupInterface_(

	);
}

extern "ASM" unsigned long alignment__PUj(void *arg1);
extern "ASM" unsigned long __saveds _PPCStub_alignment__PUj(long *args)
{
	return alignment__PUj(
		(void *) args[0]
	);
}

extern "ASM" unsigned long sor__PUj(void *arg1);
extern "ASM" unsigned long __saveds _PPCStub_sor__PUj(long *args)
{
	return sor__PUj(
		(void *) args[0]
	);
}

extern "ASM" unsigned long objectpath__PUj(void *arg1);
extern "ASM" unsigned long __saveds _PPCStub_objectpath__PUj(long *args)
{
	return objectpath__PUj(
		(void *) args[0]
	);
}

extern "ASM" unsigned long geterrorstr__PUj(void *arg1);
extern "ASM" unsigned long __saveds _PPCStub_geterrorstr__PUj(long *args)
{
	return geterrorstr__PUj(
		(void *) args[0]
	);
}

extern "ASM" unsigned long spectrans__PUj(void *arg1);
extern "ASM" unsigned long __saveds _PPCStub_spectrans__PUj(long *args)
{
	return spectrans__PUj(
		(void *) args[0]
	);
}

extern "ASM" unsigned long savepic__PUj(void *arg1);
extern "ASM" unsigned long __saveds _PPCStub_savepic__PUj(long *args)
{
	return savepic__PUj(
		(void *) args[0]
	);
}

extern "ASM" unsigned long loadobj__PUj(void *arg1);
extern "ASM" unsigned long __saveds _PPCStub_loadobj__PUj(long *args)
{
	return loadobj__PUj(
		(void *) args[0]
	);
}

extern "ASM" unsigned long newactor__PUj(void *arg1);
extern "ASM" unsigned long __saveds _PPCStub_newactor__PUj(long *args)
{
	return newactor__PUj(
		(void *) args[0]
	);
}

extern "ASM" unsigned long csg__PUj(void *arg1);
extern "ASM" unsigned long __saveds _PPCStub_csg__PUj(long *args)
{
	return csg__PUj(
		(void *) args[0]
	);
}

extern "ASM" unsigned long foglen__PUj(void *arg1);
extern "ASM" unsigned long __saveds _PPCStub_foglen__PUj(long *args)
{
	return foglen__PUj(
		(void *) args[0]
	);
}

extern "ASM" unsigned long plane__PUj(void *arg1);
extern "ASM" unsigned long __saveds _PPCStub_plane__PUj(long *args)
{
	return plane__PUj(
		(void *) args[0]
	);
}

extern "ASM" unsigned long brushpath__PUj(void *arg1);
extern "ASM" unsigned long __saveds _PPCStub_brushpath__PUj(long *args)
{
	return brushpath__PUj(
		(void *) args[0]
	);
}

