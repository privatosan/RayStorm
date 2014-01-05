/***************
 * NAME:          rsi_lib.h
 * DESCRIPTION:   Definitions for using 'raystorm.library'
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    17.06.96 ah    initial release
 *    04.11.96 ah    added rsiCreateFlare and rsiCreateStar
 *    25.11.96 ah    added rsiSetFlare and rsiSetStar
 *    05.12.96 ah    added statistic Functions
 *    10.01.97 ah    added box object functions
 *    11.01.97 ah    added CSG object functions
 *    07.03.97 ah    added rsiCreateMesh
 *    14.03.97 ah    added hyper texture
 *    19.03.97 ah    added cylinder object functions
 *    21.03.97 ah    added sor object functions
 *    21.03.97 ah    added cone object functions
 ***************/

#ifndef RSI_LIB_H
#define RSI_LIB_H

#ifndef RSI_H
#include "rsi.h"
#endif

#if defined(__PPC__) || defined(__MIXEDBINARY__)
#define RAYSTORMNAME   "raystormppc.library"
#else
#define RAYSTORMNAME   "raystorm.library"
#endif

#ifdef __PPC__

#define LIBRARY
#define LIBRARY_ONLY
/*#define LIBRARY struct Library*,
#define LIBRARY_ONLY struct Library**/
#if __STORM__ >= 39
#define LIB_OFFSET(n)
extern "library=RayStormBase"
#else
#define LIB_OFFSET(n) = n
extern "AmigaLib" RayStormBase
#endif
#else // __PPC__

#define LIBRARY
#define LIBRARY_ONLY
#define LIB_OFFSET(n) = n
extern "Amiga" RayStormBase

#endif
{
	rsiResult rsiSetCooperateCB(LIBRARY void*, void (*)(void*)) LIB_OFFSET(-0x1e);
	rsiResult rsiSetCheckCancelCB(LIBRARY void*, BOOL (*)(void*)) LIB_OFFSET(-0x24);
	rsiResult rsiSetLogCB(LIBRARY void*, void (*)(void*, char *)) LIB_OFFSET(-0x2a);
	rsiResult rsiSetUpdateStatusCB(LIBRARY void*, void (*)(void*, float, float, int, int, rsiSMALL_COLOR*)) LIB_OFFSET(-0x30);

	rsiResult rsiInit(LIBRARY_ONLY) LIB_OFFSET(-0x36);
	rsiResult rsiExit(LIBRARY_ONLY) LIB_OFFSET(-0x3c);
	rsiResult rsiCreateContext(LIBRARY void**) LIB_OFFSET(-0x42);
	rsiResult rsiFreeContext(LIBRARY void*) LIB_OFFSET(-0x48);
	rsiResult rsiSetUserData(LIBRARY void*, void*) LIB_OFFSET(-0x4e);
	rsiResult rsiGetUserData(LIBRARY void*, void**) LIB_OFFSET(-0x54);
	rsiResult rsiGetErrorMsg(LIBRARY void*, char *, int) LIB_OFFSET(-0x5a);
	rsiResult rsiCleanup(LIBRARY void*) LIB_OFFSET(-0x60);

	rsiResult rsiRender(LIBRARY void*, UBYTE **, ...) LIB_OFFSET(-0x66);
	rsiResult rsiSetOctreeDepth(LIBRARY void*, int) LIB_OFFSET(-0x6c);
	rsiResult rsiSetRenderField(LIBRARY void*, int, int, int, int) LIB_OFFSET(-0x72);
	rsiResult rsiSetAntialias(LIBRARY void*, ...) LIB_OFFSET(-0x78);
	rsiResult rsiSetDistribution(LIBRARY void*, int, int) LIB_OFFSET(-0x7e);
	rsiResult rsiSetRandomJitter(LIBRARY void*, BOOL) LIB_OFFSET(-0x84);
	rsiResult rsiSetWorld(LIBRARY void*, ...) LIB_OFFSET(-0x8a);
	rsiResult rsiSetCamera(LIBRARY void*, ...) LIB_OFFSET(-0x90);
	rsiResult rsiSetScreen(LIBRARY void*, int, int) LIB_OFFSET(-0x96);
	rsiResult rsiSetBrushPath(LIBRARY void*, char *) LIB_OFFSET(-0x9c);
	rsiResult rsiSetTexturePath(LIBRARY void*, char *) LIB_OFFSET(-0xa2);
	rsiResult rsiSetObjectPath(LIBRARY void*, char *) LIB_OFFSET(-0xa8);
	rsiResult rsiSavePicture(LIBRARY void*, char *, ...) LIB_OFFSET(-0xae);

	rsiResult rsiCreatePointLight(LIBRARY void*, void **) LIB_OFFSET(-0xb4);
	rsiResult rsiSetPointLight(LIBRARY void*, void *, ...) LIB_OFFSET(-0xba);
	rsiResult rsiCreateSpotLight(LIBRARY void*, void **) LIB_OFFSET(-0xc0);
	rsiResult rsiSetSpotLight(LIBRARY void*, void *, ...) LIB_OFFSET(-0xc6);
	rsiResult rsiCreateDirectionalLight(LIBRARY void*, void **) LIB_OFFSET(-0xcc);
	rsiResult rsiSetDirectionalLight(LIBRARY void*, void *, ...) LIB_OFFSET(-0xd2);
	rsiResult rsiCreateFlare(LIBRARY void*, void *, void *) LIB_OFFSET(-0xd8);
	rsiResult rsiSetFlare(LIBRARY void*, void *, ...) LIB_OFFSET(-0xde);
	rsiResult rsiCreateStar(LIBRARY void*, void *, void *) LIB_OFFSET(-0xe4);
	rsiResult rsiSetStar(LIBRARY void*, void *, ...) LIB_OFFSET(-0xea);
	rsiResult rsiCreateSphere(LIBRARY void*, void **, void *) LIB_OFFSET(-0xf0);
	rsiResult rsiSetSphere(LIBRARY void*, void *, ...) LIB_OFFSET(-0xf6);
	rsiResult rsiCreateTriangle(LIBRARY void*, void *, rsiVECTOR *, rsiVECTOR *, rsiVECTOR *, ...) LIB_OFFSET(-0xfc);
	rsiResult rsiCreateMesh(LIBRARY void*, void *, rsiVECTOR *, rsiEDGE *, int, rsiMESH *, int, ...) LIB_OFFSET(-0x102);
	rsiResult rsiCreatePlane(LIBRARY void*, void **,void *) LIB_OFFSET(-0x108);
	rsiResult rsiSetPlane(LIBRARY void*, void *, ...) LIB_OFFSET(-0x10e);
	rsiResult rsiCreateCSG(LIBRARY void*, void **) LIB_OFFSET(-0x114);
	rsiResult rsiSetCSG(LIBRARY void*, void *, ...) LIB_OFFSET(-0x11a);
	rsiResult rsiLoadObject(LIBRARY void*, char *, ...) LIB_OFFSET(-0x120);
	rsiResult rsiCreateBox(LIBRARY void*, void **,void *) LIB_OFFSET(-0x126);
	rsiResult rsiSetBox(LIBRARY void*, void *, ...) LIB_OFFSET(-0x12c);
	rsiResult rsiCreateCylinder(LIBRARY void*, void **,void *) LIB_OFFSET(-0x132);
	rsiResult rsiSetCylinder(LIBRARY void*, void *, ...) LIB_OFFSET(-0x138);
	rsiResult rsiCreateSOR(LIBRARY void*, void **, void *, int, rsiVECTOR2D*) LIB_OFFSET(-0x13e);
	rsiResult rsiSetSOR(LIBRARY void*, void *, ...) LIB_OFFSET(-0x144);
	rsiResult rsiCreateCone(LIBRARY void*, void **, void *) LIB_OFFSET(-0x14a);
	rsiResult rsiSetCone(LIBRARY void*, void *, ...) LIB_OFFSET(-0x150);

	rsiResult rsiCreateSurface(LIBRARY void*, void **, ...) LIB_OFFSET(-0x156);
	rsiResult rsiSetSurface(LIBRARY void*, void *, ...) LIB_OFFSET(-0x15c);
	rsiResult rsiAddImagineTexture(LIBRARY void*, void **, char *, void *) LIB_OFFSET(-0x162);
	rsiResult rsiAddRayStormTexture(LIBRARY void*, void **, char *, void *) LIB_OFFSET(-0x168);
	rsiResult rsiSetTexture(LIBRARY void*, void *, ...) LIB_OFFSET(-0x16e);
	rsiResult rsiAddHyperTexture(LIBRARY void*, void **, int, void *) LIB_OFFSET(-0x174);
	rsiResult rsiSetHyperTexture(LIBRARY void*, void *, ...) LIB_OFFSET(-0x17a);
	rsiResult rsiAddBrush(LIBRARY void*, void **, char *, void *) LIB_OFFSET(-0x180);
	rsiResult rsiSetBrush(LIBRARY void*, void *, ...) LIB_OFFSET(-0x186);

	rsiResult rsiCreateActor(LIBRARY void*, void **) LIB_OFFSET(-0x18c);
	rsiResult rsiSetActor(LIBRARY void*, void *, ...) LIB_OFFSET(-0x192);
	rsiResult rsiPosition(LIBRARY void*, void *, float, float, rsiVECTOR *, ...) LIB_OFFSET(-0x198);
	rsiResult rsiAlignment(LIBRARY void*, void *, float, float, rsiVECTOR *, ...) LIB_OFFSET(-0x19e);
	rsiResult rsiSize(LIBRARY void*, void *, float, float, rsiVECTOR *, ...) LIB_OFFSET(-0x1a4);

	rsiResult rsiGetLibInfo(rsiLIBINFO*) LIB_OFFSET(-0x1aa);

	rsiResult rsiGetStats(LIBRARY void*, rsiSTAT **, int *) LIB_OFFSET(-0x1b0);
	rsiResult rsiGetStatDescription(LIBRARY void*, rsiSTAT *, char *, int) LIB_OFFSET(-0x1b6);
}

#endif
