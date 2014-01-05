/***************
 * PROGRAM:       Modeler
 * NAME:          ppc_objhand.cpp
 * DESCRIPTION:   ppc stubs for object handler callbacks
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    25.02.98 ah    initial release
 ***************/

#include <libraries/powerpc.h>
#include <pragma/powerpc_lib.h>
extern struct Library *PowerPCBase;

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef OBJECT_H
#include "object.h"
#endif

#ifndef SURFACE_H
#include "surface.h"
#endif

#ifndef MESH_H
#include "mesh.h"
#endif

#ifndef TEXTURE_H
#include "texture.h"
#endif

#ifndef BRUSH_H
#include "brush.h"
#endif

#ifndef LIGHT_H
#include "light.h"
#endif

#ifndef CAMERA_H
#include "camera.h"
#endif

#ifndef STAR_H
#include "star.h"
#endif

#ifndef FLARES_H
#include "flares.h"
#endif

typedef void ACTOR;
typedef void IM_TEXTURE;

#ifndef OBJLINK_H
#include "objlink.h"
#endif

#include "rmod:objhand_lib.h"

extern "ASM" void __saveds _PPCStub_ppc_objCleanup_(long *args)
{
	objCleanup();
}

extern "ASM" short __saveds _PPCStub_ppc_objInit_(long *args)
{
	return objInit();
}

extern "ASM" void *__saveds _PPCStub__ppc_objRead(long *args)
{
	return objRead(
		(rsiCONTEXT*) args[0],
		(char*) args[1],
		(OBJLINK*) args[2],
		(VECTOR*) args[3],
		(VECTOR*) args[4],
		(VECTOR*) args[5],
		(VECTOR*) args[6],
		(VECTOR*) args[7],
		(ACTOR*) args[8],
		(SURFACE*) args[9],
		(ULONG*) args[10]
	);
}

void        ObjectBegin(rsiCONTEXT*);
void        ObjectEnd(rsiCONTEXT*);
void        ObjectSurface(rsiCONTEXT*, OBJECT*, SURFACE*);
BOOL        ObjectName(rsiCONTEXT*, OBJECT*, char*);
void        ObjectAxis(rsiCONTEXT*, OBJECT*, VECTOR *,VECTOR *,VECTOR *,VECTOR *,VECTOR *);
void        ObjectActor(rsiCONTEXT*, OBJECT*, ACTOR*);
void        ObjectFlags(rsiCONTEXT*, OBJECT*, ULONG);
BOOL        ObjectTrack(rsiCONTEXT*, OBJECT*, char*);
SURFACE*    SurfaceAdd(rsiCONTEXT*);
SURFACE*    SurfaceGetByName(rsiCONTEXT*, char*);
BOOL        SurfaceName(rsiCONTEXT*, SURFACE *,char *);
void        SurfaceSetFlags(rsiCONTEXT*, SURFACE *,UWORD);
void        SurfaceDiffuse(rsiCONTEXT*, SURFACE *,float,float,float);
void        SurfaceSpecular(rsiCONTEXT*, SURFACE *,float,float,float);
void        SurfaceAmbient(rsiCONTEXT*, SURFACE *,float,float,float);
void        SurfaceReflect(rsiCONTEXT*, SURFACE *,float,float,float);
void        SurfaceTranspar(rsiCONTEXT*, SURFACE *,float,float,float);
void        SurfaceDiffTrans(rsiCONTEXT*, SURFACE *,float,float,float);
void        SurfaceSpecTrans(rsiCONTEXT*, SURFACE *,float,float,float);
void        SurfaceFoglen(rsiCONTEXT*, SURFACE *,float);
float       SurfaceGetFoglen(rsiCONTEXT*, SURFACE *);
void        SurfaceIndOfRefr(rsiCONTEXT*, SURFACE *,float);
void        SurfaceRefPhong(rsiCONTEXT*, SURFACE *,float);
void        SurfaceTransPhong(rsiCONTEXT*, SURFACE *,float);
void        SurfaceTransluc(rsiCONTEXT*, SURFACE *,float);
BOOL        SurfaceCopy(rsiCONTEXT*, SURFACE *,SURFACE *);
OBJECT*     SphereAdd(rsiCONTEXT*);
void        SphereAxis(rsiCONTEXT*, OBJECT*, VECTOR*, float);
TRIANGLE*   TriangleAdd(rsiCONTEXT*, int,SURFACE *,ACTOR *);
BOOL        TriangleAddPoints(rsiCONTEXT*, int,VECTOR *);
BOOL        TriangleAddEdges(rsiCONTEXT*, int,UWORD*);
void        TriangleSetEdges(rsiCONTEXT*, TRIANGLE *,UWORD,UWORD,UWORD);
void        TriangleSetEdgeFlags(rsiCONTEXT*, UBYTE *);
void        TrianglePoints(rsiCONTEXT*, TRIANGLE *,VECTOR *,VECTOR *,VECTOR *);
BOOL        TriangleVNorm(rsiCONTEXT*, TRIANGLE *,VECTOR *,VECTOR *,VECTOR *);
void        TriangleSurface(rsiCONTEXT*, TRIANGLE*, SURFACE*);
TRIANGLE*   TriangleGetNext(rsiCONTEXT*, TRIANGLE *);
OBJECT*     MeshCreate(rsiCONTEXT*);
BOOL        MeshAddScenario(rsiCONTEXT*, OBJECT*, VECTOR*, int, LINK_EDGE*, int, LINK_MESH*, int, VECTOR*, BOOL);
OBJECT*     BoxAdd(rsiCONTEXT*);
void        BoxBounds(rsiCONTEXT*, OBJECT*, VECTOR*, VECTOR*);
void        BoxAxis(rsiCONTEXT*, OBJECT*, VECTOR*, VECTOR*, VECTOR*, VECTOR*);
OBJECT*     CylinderAdd(rsiCONTEXT*);
void        CylinderAxis(rsiCONTEXT*, OBJECT *, VECTOR *, VECTOR *, VECTOR *, VECTOR *, VECTOR *);
void        CylinderFlags(rsiCONTEXT*, OBJECT *, ULONG);
OBJECT*     ConeAdd(rsiCONTEXT*);
void        ConeAxis(rsiCONTEXT*, OBJECT*, VECTOR *, VECTOR *, VECTOR *, VECTOR *, VECTOR *);
void        ConeFlags(rsiCONTEXT*, OBJECT *, ULONG);
OBJECT*     PlaneAdd(rsiCONTEXT*);
void        PlaneAxis(rsiCONTEXT*, OBJECT*, VECTOR*, VECTOR*);
OBJECT*     CSGAdd(rsiCONTEXT*);
void        CSGOperation(rsiCONTEXT*, OBJECT*, UWORD);
OBJECT*     SORAdd(rsiCONTEXT*);
void        SORAxis(rsiCONTEXT*, OBJECT*, VECTOR *, VECTOR *, VECTOR *, VECTOR *, VECTOR *);
void        SORFlags(rsiCONTEXT*, OBJECT *, ULONG);
BOOL        SORPoints(rsiCONTEXT*, OBJECT*, VECT2D*, ULONG);
TEXTURE*    ITextureAdd(rsiCONTEXT*, char *,SURFACE *);
void        ITextureParams(rsiCONTEXT*, TEXTURE *,int,float);
TEXTURE*    RSTextureAdd(rsiCONTEXT*, char *,SURFACE *);
BOOL        RSTextureParams(rsiCONTEXT*, TEXTURE *, int, UBYTE*);
TEXTURE_OBJECT*TextureObjectAdd(rsiCONTEXT*, OBJECT*, ACTOR*, TEXTURE_OBJECT*);
void        TexturePos(rsiCONTEXT*, TEXTURE_OBJECT *,VECTOR *);
void        TextureOrient(rsiCONTEXT*, TEXTURE_OBJECT *,VECTOR *,VECTOR *,VECTOR *);
void        TextureSize(rsiCONTEXT*, TEXTURE_OBJECT *,VECTOR *);
BRUSH*      BrushAdd(rsiCONTEXT*, char *,SURFACE *);
BRUSH_OBJECT*BrushObjectAdd(rsiCONTEXT*, OBJECT*, ACTOR*, BRUSH_OBJECT*);
void        BrushFlags(rsiCONTEXT*, BRUSH *,ULONG);
void        BrushPos(rsiCONTEXT*, BRUSH_OBJECT *,VECTOR *);
void        BrushOrient(rsiCONTEXT*, BRUSH_OBJECT *,VECTOR *,VECTOR *,VECTOR *);
void        BrushSize(rsiCONTEXT*, BRUSH_OBJECT *,VECTOR *);
void        BrushAlignment(rsiCONTEXT*, BRUSH_OBJECT *,VECTOR *);
CAMERA*     CameraAdd(rsiCONTEXT*);
void        CameraFlags(rsiCONTEXT*, CAMERA*, ULONG);
void        CameraFocalDist(rsiCONTEXT*, CAMERA*, float);
void        CameraAperture(rsiCONTEXT*, CAMERA*, float);
void        CameraFOV(rsiCONTEXT*, CAMERA*, float, float);
void        LightPos(rsiCONTEXT*, LIGHT*, VECTOR *);
void        LightColor(rsiCONTEXT*, LIGHT*, float, float, float);
void        LightFlags(rsiCONTEXT*, LIGHT*, ULONG);
void        LightFallOff(rsiCONTEXT*, LIGHT*, float);
void        LightRadius(rsiCONTEXT*, LIGHT*, float);
void        LightShadowMapSize(rsiCONTEXT*, LIGHT*, int);
LIGHT*      PointLightAdd(rsiCONTEXT*);
LIGHT*      SpotLightAdd(rsiCONTEXT*);
void        SpotLightAngle(rsiCONTEXT*, LIGHT*, float);
void        SpotLightFallOffRadius(rsiCONTEXT*, LIGHT*, float);
LIGHT*      DirectionalLightAdd(rsiCONTEXT*);
FLARE*      FlareAdd(rsiCONTEXT*, LIGHT *);
void        FlareColor(rsiCONTEXT*, FLARE*, float, float, float);
void        FlarePosition(rsiCONTEXT*, FLARE*, float);
void        FlareRadius(rsiCONTEXT*, FLARE*, float);
void        FlareType(rsiCONTEXT*, FLARE*, ULONG);
void        FlareFunction(rsiCONTEXT*, FLARE*, ULONG);
void        FlareEdges(rsiCONTEXT*, FLARE*, ULONG);
void        FlareTilt(rsiCONTEXT*, FLARE*, float);
STAR*       StarAdd(rsiCONTEXT*, LIGHT*);
void        StarFlags(rsiCONTEXT*, STAR*, ULONG);
void        StarRadius(rsiCONTEXT*, STAR*, float);
void        StarTilt(rsiCONTEXT*, STAR*, float);
void        StarSpikes(rsiCONTEXT*, STAR*, ULONG);
void        StarHaloradius(rsiCONTEXT*, STAR*, float);
void        StarInnerHaloradius(rsiCONTEXT*, STAR*, float);
void        StarRange(rsiCONTEXT*, STAR*, float);
BOOL        StarIntensities(rsiCONTEXT*, STAR*, int, float*);

extern "ASM" void _68kStub_DirectionalLightAdd__Pv(void);
extern "ASM" void *DirectionalLightAdd__Pv(void *arg1)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_DirectionalLightAdd__Pv)(void) = _68kStub_DirectionalLightAdd__Pv;
	ppc.PP_Code = _68kData_DirectionalLightAdd__Pv;
	long args[1];
	args[0] = (long) arg1;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
	return (void *) ppc.PP_Regs[0];
}

extern "ASM" void _68kStub_SpotLightAdd__Pv(void);
extern "ASM" void *SpotLightAdd__Pv(void *arg1)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_SpotLightAdd__Pv)(void) = _68kStub_SpotLightAdd__Pv;
	ppc.PP_Code = _68kData_SpotLightAdd__Pv;
	long args[1];
	args[0] = (long) arg1;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
	return (void *) ppc.PP_Regs[0];
}
extern "ASM" void _68kStub_PointLightAdd__Pv(void);
extern "ASM" void *PointLightAdd__Pv(void *arg1)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_PointLightAdd__Pv)(void) = _68kStub_PointLightAdd__Pv;
	ppc.PP_Code = _68kData_PointLightAdd__Pv;
	long args[1];
	args[0] = (long) arg1;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
	return (void *) ppc.PP_Regs[0];
}
extern "ASM" void _68kStub_StarIntensities__PvP04STARiPf(void);
extern "ASM" short StarIntensities__PvP04STARiPf(void *arg1,void *arg2,long arg3,void *arg4)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_StarIntensities__PvP04STARiPf)(void) = _68kStub_StarIntensities__PvP04STARiPf;
	ppc.PP_Code = _68kData_StarIntensities__PvP04STARiPf;
	long args[4];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) arg3;
	args[3] = (long) arg4;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
	return (short) ppc.PP_Regs[0];
}
extern "ASM" void _68kStub_StarAdd__PvP05LIGHT(void);
extern "ASM" void *StarAdd__PvP05LIGHT(void *arg1,void *arg2)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_StarAdd__PvP05LIGHT)(void) = _68kStub_StarAdd__PvP05LIGHT;
	ppc.PP_Code = _68kData_StarAdd__PvP05LIGHT;
	long args[2];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
	return (void *) ppc.PP_Regs[0];
}
extern "ASM" void _68kStub_FlareAdd__PvP05LIGHT(void);
extern "ASM" void *FlareAdd__PvP05LIGHT(void *arg1,void *arg2)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_FlareAdd__PvP05LIGHT)(void) = _68kStub_FlareAdd__PvP05LIGHT;
	ppc.PP_Code = _68kData_FlareAdd__PvP05LIGHT;
	long args[2];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
	return (void *) ppc.PP_Regs[0];
}
extern "ASM" void _68kStub_CameraAdd__Pv(void);
extern "ASM" void *CameraAdd__Pv(void *arg1)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_CameraAdd__Pv)(void) = _68kStub_CameraAdd__Pv;
	ppc.PP_Code = _68kData_CameraAdd__Pv;
	long args[1];
	args[0] = (long) arg1;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
	return (void *) ppc.PP_Regs[0];
}
extern "ASM" void _68kStub_BrushSize__PvP12BRUSH_OBJECTP06VECTOR(void);
extern "ASM" void BrushSize__PvP12BRUSH_OBJECTP06VECTOR(void *arg1,void *arg2,void *arg3)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_BrushSize__PvP12BRUSH_OBJECTP06VECTOR)(void) = _68kStub_BrushSize__PvP12BRUSH_OBJECTP06VECTOR;
	ppc.PP_Code = _68kData_BrushSize__PvP12BRUSH_OBJECTP06VECTOR;
	long args[3];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) arg3;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
}
extern "ASM" void _68kStub_BrushOrient__PvP12BRUSH_OBJECTP06VECTORP06VECTORP06VECTOR(void);
extern "ASM" void BrushOrient__PvP12BRUSH_OBJECTP06VECTORP06VECTORP06VECTOR(void *arg1,void *arg2,void *arg3,void *arg4,void *arg5)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_BrushOrient__PvP12BRUSH_OBJECTP06VECTORP06VECTORP06VECTOR)(void) = _68kStub_BrushOrient__PvP12BRUSH_OBJECTP06VECTORP06VECTORP06VECTOR;
	ppc.PP_Code = _68kData_BrushOrient__PvP12BRUSH_OBJECTP06VECTORP06VECTORP06VECTOR;
	long args[5];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) arg3;
	args[3] = (long) arg4;
	args[4] = (long) arg5;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
}
extern "ASM" void _68kStub_BrushPos__PvP12BRUSH_OBJECTP06VECTOR(void);
extern "ASM" void BrushPos__PvP12BRUSH_OBJECTP06VECTOR(void *arg1,void *arg2,void *arg3)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_BrushPos__PvP12BRUSH_OBJECTP06VECTOR)(void) = _68kStub_BrushPos__PvP12BRUSH_OBJECTP06VECTOR;
	ppc.PP_Code = _68kData_BrushPos__PvP12BRUSH_OBJECTP06VECTOR;
	long args[3];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) arg3;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
}
extern "ASM" void _68kStub_BrushObjectAdd__PvP06OBJECTPvP12BRUSH_OBJECT(void);
extern "ASM" void *BrushObjectAdd__PvP06OBJECTPvP12BRUSH_OBJECT(void *arg1,void *arg2,void *arg3,void *arg4)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_BrushObjectAdd__PvP06OBJECTPvP12BRUSH_OBJECT)(void) = _68kStub_BrushObjectAdd__PvP06OBJECTPvP12BRUSH_OBJECT;
	ppc.PP_Code = _68kData_BrushObjectAdd__PvP06OBJECTPvP12BRUSH_OBJECT;
	long args[4];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) arg3;
	args[3] = (long) arg4;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
	return (void *) ppc.PP_Regs[0];
}
extern "ASM" void _68kStub_BrushAdd__PvPcP07SURFACE(void);
extern "ASM" void *BrushAdd__PvPcP07SURFACE(void *arg1,void *arg2,void *arg3)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_BrushAdd__PvPcP07SURFACE)(void) = _68kStub_BrushAdd__PvPcP07SURFACE;
	ppc.PP_Code = _68kData_BrushAdd__PvPcP07SURFACE;
	long args[3];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) arg3;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
	return (void *) ppc.PP_Regs[0];
}
extern struct Library *PowerPCBase;
extern "ASM" void _68kStub_TextureSize__PvP14TEXTURE_OBJECTP06VECTOR(void);
extern "ASM" void TextureSize__PvP14TEXTURE_OBJECTP06VECTOR(void *arg1,void *arg2,void *arg3)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_TextureSize__PvP14TEXTURE_OBJECTP06VECTOR)(void) = _68kStub_TextureSize__PvP14TEXTURE_OBJECTP06VECTOR;
	ppc.PP_Code = _68kData_TextureSize__PvP14TEXTURE_OBJECTP06VECTOR;
	long args[3];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) arg3;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
}
extern "ASM" void _68kStub_TextureObjectAdd__PvP06OBJECTPvP14TEXTURE_OBJECT(void);
extern "ASM" void *TextureObjectAdd__PvP06OBJECTPvP14TEXTURE_OBJECT(void *arg1,void *arg2,void *arg3,void *arg4)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_TextureObjectAdd__PvP06OBJECTPvP14TEXTURE_OBJECT)(void) = _68kStub_TextureObjectAdd__PvP06OBJECTPvP14TEXTURE_OBJECT;
	ppc.PP_Code = _68kData_TextureObjectAdd__PvP06OBJECTPvP14TEXTURE_OBJECT;
	long args[4];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) arg3;
	args[3] = (long) arg4;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
	return (void *) ppc.PP_Regs[0];
}
extern "ASM" void _68kStub_RSTextureParams__PvP07TEXTUREiPUc(void);
extern "ASM" short RSTextureParams__PvP07TEXTUREiPUc(void *arg1,void *arg2,long arg3,void *arg4)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_RSTextureParams__PvP07TEXTUREiPUc)(void) = _68kStub_RSTextureParams__PvP07TEXTUREiPUc;
	ppc.PP_Code = _68kData_RSTextureParams__PvP07TEXTUREiPUc;
	long args[4];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) arg3;
	args[3] = (long) arg4;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
	return (short) ppc.PP_Regs[0];
}
extern "ASM" void _68kStub_RSTextureAdd__PvPcP07SURFACE(void);
extern "ASM" void *RSTextureAdd__PvPcP07SURFACE(void *arg1,void *arg2,void *arg3)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_RSTextureAdd__PvPcP07SURFACE)(void) = _68kStub_RSTextureAdd__PvPcP07SURFACE;
	ppc.PP_Code = _68kData_RSTextureAdd__PvPcP07SURFACE;
	long args[3];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) arg3;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
	return (void *) ppc.PP_Regs[0];
}
extern "ASM" void _68kStub_ITextureAdd__PvPcP07SURFACE(void);
extern "ASM" void *ITextureAdd__PvPcP07SURFACE(void *arg1,void *arg2,void *arg3)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_ITextureAdd__PvPcP07SURFACE)(void) = _68kStub_ITextureAdd__PvPcP07SURFACE;
	ppc.PP_Code = _68kData_ITextureAdd__PvPcP07SURFACE;
	long args[3];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) arg3;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
	return (void *) ppc.PP_Regs[0];
}
extern "ASM" void _68kStub_SORPoints__PvP06OBJECTP06VECT2DUj(void);
extern "ASM" short SORPoints__PvP06OBJECTP06VECT2DUj(void *arg1,void *arg2,void *arg3,unsigned long arg4)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_SORPoints__PvP06OBJECTP06VECT2DUj)(void) = _68kStub_SORPoints__PvP06OBJECTP06VECT2DUj;
	ppc.PP_Code = _68kData_SORPoints__PvP06OBJECTP06VECT2DUj;
	long args[4];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) arg3;
	args[3] = (long) arg4;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
	return (short) ppc.PP_Regs[0];
}
extern "ASM" void _68kStub_SORAdd__Pv(void);
extern "ASM" void *SORAdd__Pv(void *arg1)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_SORAdd__Pv)(void) = _68kStub_SORAdd__Pv;
	ppc.PP_Code = _68kData_SORAdd__Pv;
	long args[1];
	args[0] = (long) arg1;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
	return (void *) ppc.PP_Regs[0];
}
extern "ASM" void _68kStub_CSGAdd__Pv(void);
extern "ASM" void *CSGAdd__Pv(void *arg1)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_CSGAdd__Pv)(void) = _68kStub_CSGAdd__Pv;
	ppc.PP_Code = _68kData_CSGAdd__Pv;
	long args[1];
	args[0] = (long) arg1;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
	return (void *) ppc.PP_Regs[0];
}
extern "ASM" void _68kStub_PlaneAdd__Pv(void);
extern "ASM" void *PlaneAdd__Pv(void *arg1)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_PlaneAdd__Pv)(void) = _68kStub_PlaneAdd__Pv;
	ppc.PP_Code = _68kData_PlaneAdd__Pv;
	long args[1];
	args[0] = (long) arg1;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
	return (void *) ppc.PP_Regs[0];
}
extern "ASM" void _68kStub_ConeAdd__Pv(void);
extern "ASM" void *ConeAdd__Pv(void *arg1)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_ConeAdd__Pv)(void) = _68kStub_ConeAdd__Pv;
	ppc.PP_Code = _68kData_ConeAdd__Pv;
	long args[1];
	args[0] = (long) arg1;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
	return (void *) ppc.PP_Regs[0];
}
extern "ASM" void _68kStub_CylinderAdd__Pv(void);
extern "ASM" void *CylinderAdd__Pv(void *arg1)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_CylinderAdd__Pv)(void) = _68kStub_CylinderAdd__Pv;
	ppc.PP_Code = _68kData_CylinderAdd__Pv;
	long args[1];
	args[0] = (long) arg1;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
	return (void *) ppc.PP_Regs[0];
}
extern "ASM" void _68kStub_BoxAdd__Pv(void);
extern "ASM" void *BoxAdd__Pv(void *arg1)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_BoxAdd__Pv)(void) = _68kStub_BoxAdd__Pv;
	ppc.PP_Code = _68kData_BoxAdd__Pv;
	long args[1];
	args[0] = (long) arg1;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
	return (void *) ppc.PP_Regs[0];
}
extern "ASM" void _68kStub_MeshCreate__Pv(void);
extern "ASM" void *MeshCreate__Pv(void *arg1)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_MeshCreate__Pv)(void) = _68kStub_MeshCreate__Pv;
	ppc.PP_Code = _68kData_MeshCreate__Pv;
	long args[1];
	args[0] = (long) arg1;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
	return (void *) ppc.PP_Regs[0];
}
extern "ASM" void _68kStub_TriangleSetEdgeFlags__PvPUc(void);
extern "ASM" void TriangleSetEdgeFlags__PvPUc(void *arg1,void *arg2)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_TriangleSetEdgeFlags__PvPUc)(void) = _68kStub_TriangleSetEdgeFlags__PvPUc;
	ppc.PP_Code = _68kData_TriangleSetEdgeFlags__PvPUc;
	long args[2];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
}
extern "ASM" void _68kStub_TriangleAddEdges__PviPUs(void);
extern "ASM" short TriangleAddEdges__PviPUs(void *arg1,long arg2,void *arg3)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_TriangleAddEdges__PviPUs)(void) = _68kStub_TriangleAddEdges__PviPUs;
	ppc.PP_Code = _68kData_TriangleAddEdges__PviPUs;
	long args[3];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) arg3;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
	return (short) ppc.PP_Regs[0];
}
extern "ASM" void _68kStub_TriangleAddPoints__PviP06VECTOR(void);
extern "ASM" short TriangleAddPoints__PviP06VECTOR(void *arg1,long arg2,void *arg3)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_TriangleAddPoints__PviP06VECTOR)(void) = _68kStub_TriangleAddPoints__PviP06VECTOR;
	ppc.PP_Code = _68kData_TriangleAddPoints__PviP06VECTOR;
	long args[3];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) arg3;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
	return (short) ppc.PP_Regs[0];
}
extern "ASM" void _68kStub_TriangleAdd__PviP07SURFACEPv(void);
extern "ASM" void *TriangleAdd__PviP07SURFACEPv(void *arg1,long arg2,void *arg3,void *arg4)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_TriangleAdd__PviP07SURFACEPv)(void) = _68kStub_TriangleAdd__PviP07SURFACEPv;
	ppc.PP_Code = _68kData_TriangleAdd__PviP07SURFACEPv;
	long args[4];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) arg3;
	args[3] = (long) arg4;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
	return (void *) ppc.PP_Regs[0];
}
extern "ASM" void _68kStub_SphereAdd__Pv(void);
extern "ASM" void *SphereAdd__Pv(void *arg1)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_SphereAdd__Pv)(void) = _68kStub_SphereAdd__Pv;
	ppc.PP_Code = _68kData_SphereAdd__Pv;
	long args[1];
	args[0] = (long) arg1;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
	return (void *) ppc.PP_Regs[0];
}
extern "ASM" void _68kStub_SurfaceCopy__PvP07SURFACEP07SURFACE(void);
extern "ASM" short SurfaceCopy__PvP07SURFACEP07SURFACE(void *arg1,void *arg2,void *arg3)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_SurfaceCopy__PvP07SURFACEP07SURFACE)(void) = _68kStub_SurfaceCopy__PvP07SURFACEP07SURFACE;
	ppc.PP_Code = _68kData_SurfaceCopy__PvP07SURFACEP07SURFACE;
	long args[3];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) arg3;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
	return (short) ppc.PP_Regs[0];
}
extern "ASM" void _68kStub_SurfaceGetByName__PvPc(void);
extern "ASM" void *SurfaceGetByName__PvPc(void *arg1,void *arg2)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_SurfaceGetByName__PvPc)(void) = _68kStub_SurfaceGetByName__PvPc;
	ppc.PP_Code = _68kData_SurfaceGetByName__PvPc;
	long args[2];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
	return (void *) ppc.PP_Regs[0];
}
extern "ASM" void _68kStub_SurfaceAdd__Pv(void);
extern "ASM" void *SurfaceAdd__Pv(void *arg1)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_SurfaceAdd__Pv)(void) = _68kStub_SurfaceAdd__Pv;
	ppc.PP_Code = _68kData_SurfaceAdd__Pv;
	long args[1];
	args[0] = (long) arg1;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
	return (void *) ppc.PP_Regs[0];
}
extern "ASM" void _68kStub_ObjectTrack__PvP06OBJECTPc(void);
extern "ASM" short ObjectTrack__PvP06OBJECTPc(void *arg1,void *arg2,void *arg3)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_ObjectTrack__PvP06OBJECTPc)(void) = _68kStub_ObjectTrack__PvP06OBJECTPc;
	ppc.PP_Code = _68kData_ObjectTrack__PvP06OBJECTPc;
	long args[3];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) arg3;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
	return (short) ppc.PP_Regs[0];
}
extern "ASM" void _68kStub_ObjectAxis__PvP06OBJECTP06VECTORP06VECTORP06VECTORP06VECTORP06VECTOR(void);
extern "ASM" void ObjectAxis__PvP06OBJECTP06VECTORP06VECTORP06VECTORP06VECTORP06VECTOR(void *arg1,void *arg2,void *arg3,void *arg4,void *arg5,void *arg6,void *arg7)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_ObjectAxis__PvP06OBJECTP06VECTORP06VECTORP06VECTORP06VECTORP06VECTOR)(void) = _68kStub_ObjectAxis__PvP06OBJECTP06VECTORP06VECTORP06VECTORP06VECTORP06VECTOR;
	ppc.PP_Code = _68kData_ObjectAxis__PvP06OBJECTP06VECTORP06VECTORP06VECTORP06VECTORP06VECTOR;
	long args[7];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) arg3;
	args[3] = (long) arg4;
	args[4] = (long) arg5;
	args[5] = (long) arg6;
	args[6] = (long) arg7;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
}
extern "ASM" void _68kStub_ObjectName__PvP06OBJECTPc(void);
extern "ASM" short ObjectName__PvP06OBJECTPc(void *arg1,void *arg2,void *arg3)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_ObjectName__PvP06OBJECTPc)(void) = _68kStub_ObjectName__PvP06OBJECTPc;
	ppc.PP_Code = _68kData_ObjectName__PvP06OBJECTPc;
	long args[3];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) arg3;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
	return (short) ppc.PP_Regs[0];
}
extern "ASM" void _68kStub_ObjectEnd__Pv(void);
extern "ASM" void ObjectEnd__Pv(void *arg1)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_ObjectEnd__Pv)(void) = _68kStub_ObjectEnd__Pv;
	ppc.PP_Code = _68kData_ObjectEnd__Pv;
	long args[1];
	args[0] = (long) arg1;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
}
extern "ASM" void _68kStub_SurfaceName__PvP07SURFACEPc(void);
extern "ASM" short SurfaceName__PvP07SURFACEPc(void *arg1,void *arg2,void *arg3)
{
	struct PPCArgs ppc = { };
	static void (*_68kData_SurfaceName__PvP07SURFACEPc)(void) = _68kStub_SurfaceName__PvP07SURFACEPc;
	ppc.PP_Code = _68kData_SurfaceName__PvP07SURFACEPc;
	long args[3];
	args[0] = (long) arg1;
	args[1] = (long) arg2;
	args[2] = (long) arg3;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
	return (short) ppc.PP_Regs[0];
}

static void __saveds ppc_ObjectEnd(rsiCONTEXT *rc)
{
	ObjectEnd(rc);
}

static BOOL __saveds ppc_ObjectName(rsiCONTEXT *rc, OBJECT *obj, char *name)
{
	return ObjectName(rc,obj,name);
}

static void __saveds ppc_ObjectAxis(rsiCONTEXT *rc, OBJECT *obj, VECTOR *pos, VECTOR *orient_x, VECTOR *orient_y, VECTOR *orient_z, VECTOR *size)
{
	ObjectAxis(rc,obj,pos,orient_x,orient_y,orient_z,size);
}

static BOOL __saveds ppc_ObjectTrack(rsiCONTEXT *rc, OBJECT *object, char *track)
{
	return ObjectTrack(rc,object,track);
}

static SURFACE* __saveds ppc_SurfaceAdd(rsiCONTEXT *rc)
{
	return SurfaceAdd(rc);
}

static SURFACE* __saveds ppc_SurfaceGetByName(rsiCONTEXT *rc, char *name)
{
	return SurfaceGetByName(rc,name);
}

static BOOL __saveds ppc_SurfaceName(rsiCONTEXT *rc, SURFACE *surf,char *name)
{
	return SurfaceName(rc,surf,name);
}

static BOOL __saveds ppc_SurfaceCopy(rsiCONTEXT *rc, SURFACE *from,SURFACE *to)
{
	return SurfaceCopy(rc,from,to);
}

static OBJECT* __saveds ppc_SphereAdd(rsiCONTEXT *rc)
{
	return SphereAdd(rc);
}

static TRIANGLE* __saveds ppc_TriangleAdd(rsiCONTEXT *rc, int count,SURFACE *surf,ACTOR *actor)
{
	return TriangleAdd(rc,count,surf,actor);
}

static BOOL __saveds ppc_TriangleAddPoints(rsiCONTEXT *rc, int count, VECTOR *points)
{
	return TriangleAddPoints(rc,count,points);
}

static BOOL __saveds ppc_TriangleAddEdges(rsiCONTEXT *rc, int count, UWORD *edges)
{
	return TriangleAddEdges(rc,count,edges);
}

static void __saveds ppc_TriangleSetEdgeFlags(rsiCONTEXT *rc, UBYTE *flags)
{
	TriangleSetEdgeFlags(rc,flags);
}

static OBJECT* __saveds ppc_MeshCreate(rsiCONTEXT *rc)
{
	return MeshCreate(rc);
}

extern "ASM" void _68kStub_MeshAddScenario__PvP06OBJECTP06VECTORiP09LINK_EDGEiP09LINK_MESHiP06VECTORs(void);
static BOOL __saveds ppc_MeshAddScenario(rsiCONTEXT *rc, OBJECT *mesh, VECTOR *points, int pointcount,
	LINK_EDGE *edges, int edgecount, LINK_MESH *trias, int triacount,
	VECTOR *size, BOOL nophong)
{
	long args[10] =
	{
		(long) rc,
		(long) mesh,
		(long) points,
		(long) pointcount,
		(long) edges,
		(long) edgecount,
		(long) trias,
		(long) triacount,
		(long) size,
		(long) nophong
	};
	struct PPCArgs ppc = { };
	static void (*_68kData_MeshAddScenario__PvP06OBJECTP06VECTORiP09LINK_EDGEiP09LINK_MESHiP06VECTORs)(void) = _68kStub_MeshAddScenario__PvP06OBJECTP06VECTORiP09LINK_EDGEiP09LINK_MESHiP06VECTORs;
	ppc.PP_Code = _68kData_MeshAddScenario__PvP06OBJECTP06VECTORiP09LINK_EDGEiP09LINK_MESHiP06VECTORs;
	ppc.PP_Regs[8] = (ULONG) args;
	Run68K(&ppc);
	return (BOOL) ppc.PP_Regs[0];
}

static OBJECT* __saveds ppc_BoxAdd(rsiCONTEXT *rc)
{
	return BoxAdd(rc);
}

static OBJECT* __saveds ppc_CylinderAdd(rsiCONTEXT *rc)
{
	return CylinderAdd(rc);
}

static OBJECT* __saveds ppc_ConeAdd(rsiCONTEXT *rc)
{
	return ConeAdd(rc);
}

static OBJECT* __saveds ppc_PlaneAdd(rsiCONTEXT *rc)
{
	return PlaneAdd(rc);
}

static OBJECT* __saveds ppc_CSGAdd(rsiCONTEXT *rc)
{
	return CSGAdd(rc);
}

static OBJECT* __saveds ppc_SORAdd(rsiCONTEXT *rc)
{
	return SORAdd(rc);
}

static BOOL __saveds ppc_SORPoints(rsiCONTEXT *rc, OBJECT *sor, VECT2D *points, ULONG count)
{
	return SORPoints(rc,sor,points,count);
}

static TEXTURE* __saveds ppc_ITextureAdd(rsiCONTEXT *rc, char *name,SURFACE *surf)
{
	return ITextureAdd(rc,name,surf);
}

static TEXTURE* __saveds ppc_RSTextureAdd(rsiCONTEXT *rc, char *name,SURFACE *surf)
{
	return RSTextureAdd(rc,name,surf);
}

static BOOL __saveds ppc_RSTextureParams(rsiCONTEXT *rc, TEXTURE *texture, int size, UBYTE *params)
{
	return RSTextureParams(rc, texture, size, params);
}

static TEXTURE_OBJECT* __saveds ppc_TextureObjectAdd(rsiCONTEXT *rc, OBJECT *obj, ACTOR *actor, TEXTURE_OBJECT *prev)
{
	return TextureObjectAdd(rc,obj,actor,prev);
}

static void __saveds ppc_TextureSize(rsiCONTEXT *rc, TEXTURE_OBJECT *texture,VECTOR *size)
{
	TextureSize(rc,texture,size);
}

static BRUSH* __saveds ppc_BrushAdd(rsiCONTEXT *rc, char *name,SURFACE *surf)
{
	return BrushAdd(rc,name,surf);
}

static BRUSH_OBJECT* __saveds ppc_BrushObjectAdd(rsiCONTEXT *rc, OBJECT *obj, ACTOR *actor, BRUSH_OBJECT *prev)
{
	return BrushObjectAdd(rc,obj,actor,prev);
}

static void __saveds ppc_BrushPos(rsiCONTEXT *rc, BRUSH_OBJECT *brush,VECTOR *pos)
{
	BrushPos(rc,brush,pos);
}

static void __saveds ppc_BrushOrient(rsiCONTEXT *rc, BRUSH_OBJECT *brush,VECTOR *orient_x,VECTOR *orient_y,VECTOR *orient_z)
{
	BrushOrient(rc,brush,orient_x,orient_y,orient_z);
}

static void __saveds ppc_BrushSize(rsiCONTEXT *rc, BRUSH_OBJECT *brush,VECTOR *size)
{
	BrushSize(rc,brush,size);
}

static CAMERA* __saveds ppc_CameraAdd(rsiCONTEXT *rc)
{
	return CameraAdd(rc);
}

static LIGHT* __saveds ppc_PointLightAdd(rsiCONTEXT *rc)
{
	return PointLightAdd(rc);
}

static LIGHT* __saveds ppc_SpotLightAdd(rsiCONTEXT *rc)
{
	return SpotLightAdd(rc);
}

static LIGHT* __saveds ppc_DirectionalLightAdd(rsiCONTEXT *rc)
{
	return DirectionalLightAdd(rc);
}

static FLARE* __saveds ppc_FlareAdd(rsiCONTEXT *rc, LIGHT *light)
{
	return FlareAdd(rc,light);
}

static STAR* __saveds ppc_StarAdd(rsiCONTEXT *rc, LIGHT *light)
{
	return StarAdd(rc,light);
}

static BOOL __saveds ppc_StarIntensities(rsiCONTEXT *rc, STAR *star, int num, float *inte)
{
	return StarIntensities(rc,star,num,inte);
}

OBJLINK link =
{
	LINK_SCENARIO,

	ObjectBegin,
	ppc_ObjectEnd,
	ObjectSurface,
	ppc_ObjectName,
	ppc_ObjectAxis,
	NULL,             // ObjectActor
	ObjectFlags,
	ppc_ObjectTrack,

	ppc_SurfaceAdd,
	ppc_SurfaceGetByName,
	ppc_SurfaceName,
	SurfaceSetFlags,
	SurfaceDiffuse,
	SurfaceSpecular,
	SurfaceAmbient,
	SurfaceReflect,
	SurfaceTranspar,
	SurfaceDiffTrans,
	SurfaceSpecTrans,
	SurfaceFoglen,
	SurfaceGetFoglen,
	SurfaceIndOfRefr,
	SurfaceRefPhong,
	SurfaceTransPhong,
	SurfaceTransluc,
	ppc_SurfaceCopy,

	ppc_SphereAdd,
	NULL,             // SphereAxis

	ppc_TriangleAdd,
	ppc_TriangleAddPoints,
	ppc_TriangleAddEdges,
	TriangleSetEdges,
	ppc_TriangleSetEdgeFlags,
	NULL,             // TrianglePoints
	TriangleVNorm,
	NULL,             // TriangleUV
	TriangleSurface,
	TriangleGetNext,

	ppc_MeshCreate,
	ppc_MeshAddScenario,
	NULL,             // MeshAddRenderer

	ppc_BoxAdd,
	BoxBounds,
	NULL,             // BoxPos

	ppc_CylinderAdd,
	NULL,             // CylinderAxis
	CylinderFlags,

	ppc_ConeAdd,
	NULL,             // ConeAxis
	ConeFlags,

	ppc_PlaneAdd,
	NULL,             // PlaneAxis

	ppc_CSGAdd,
	CSGOperation,

	ppc_SORAdd,
	NULL,             // SORAxis
	SORFlags,
	ppc_SORPoints,

	ppc_ITextureAdd,
	ITextureParams,

	ppc_RSTextureAdd,
	ppc_RSTextureParams,

	ppc_TextureObjectAdd,
	TexturePos,
	TextureOrient,
	ppc_TextureSize,

	ppc_BrushAdd,
	ppc_BrushObjectAdd,
	BrushFlags,
	ppc_BrushPos,
	ppc_BrushOrient,
	ppc_BrushSize,
	BrushAlignment,

	ppc_CameraAdd,
	CameraFlags,
	CameraFocalDist,
	CameraAperture,
	CameraFOV,

	NULL,             // Light_Pos
	LightColor,
	LightFlags,
	LightFallOff,
	LightRadius,
	LightShadowMapSize,

	ppc_PointLightAdd,

	ppc_SpotLightAdd,
	SpotLightAngle,
	SpotLightFallOffRadius,

	ppc_DirectionalLightAdd,

	ppc_FlareAdd,
	FlareColor,
	FlarePosition,
	FlareRadius,
	FlareType,
	FlareFunction,
	FlareEdges,
	FlareTilt,

	ppc_StarAdd,
	StarFlags,
	StarRadius,
	StarTilt,
	StarSpikes,
	StarHaloradius,
	StarInnerHaloradius,
	StarRange,
	ppc_StarIntensities
};
