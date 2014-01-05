#ifndef PPC_RSI_STUBS_H
#define PPC_RSI_STUBS_H

#ifndef RSI_H
#include "rsi.h"
#endif

#ifndef RSI_LIB_H
#include "rsi_lib.h"
#endif

int ppcRayStormLibOpen();
void ppcRayStormLibClose();
rsiResult ppcSetCallbacks(rsiCONTEXT*);

rsiResult stub_rsiRender(rsiCONTEXT*, UBYTE**, ...);
rsiResult stub_rsiSetAntialias(rsiCONTEXT*, ...);
rsiResult stub_rsiSetWorld(rsiCONTEXT*, ...);
rsiResult stub_rsiSetCamera(rsiCONTEXT*, ...);
rsiResult stub_rsiSavePicture(rsiCONTEXT*, char*, ...);
rsiResult stub_rsiSetPointLight(rsiCONTEXT*, void*, ...);
rsiResult stub_rsiSetSpotLight(rsiCONTEXT*, void*, ...);
rsiResult stub_rsiSetDirectionalLight(rsiCONTEXT*, void*, ...);
rsiResult stub_rsiSetFlare(rsiCONTEXT*, void*, ...);
rsiResult stub_rsiSetStar(rsiCONTEXT*, void*, ...);
rsiResult stub_rsiSetSphere(rsiCONTEXT*, void*, ...);
rsiResult stub_rsiSetBox(rsiCONTEXT*, void*, ...);
rsiResult stub_rsiSetCylinder(rsiCONTEXT*, void*, ...);
rsiResult stub_rsiSetSOR(rsiCONTEXT*, void*, ...);
rsiResult stub_rsiSetCone(rsiCONTEXT*, void*, ...);
rsiResult stub_rsiCreateTriangle(rsiCONTEXT*, void*, rsiVECTOR*, rsiVECTOR*, rsiVECTOR*, ...);
rsiResult stub_rsiCreateMesh(rsiCONTEXT*, void*, rsiVECTOR*, rsiEDGE*, int, rsiMESH*, int, ...);
rsiResult stub_rsiSetPlane(rsiCONTEXT*, void*, ...);
rsiResult stub_rsiSetCSG(rsiCONTEXT*, void*, ...);
rsiResult stub_rsiLoadObject(rsiCONTEXT*, char*, ...);
rsiResult stub_rsiCreateSurface(rsiCONTEXT*, void**, ...);
rsiResult stub_rsiSetSurface(rsiCONTEXT*, void*, ...);
rsiResult stub_rsiSetTexture(rsiCONTEXT*, void*, ...);
rsiResult stub_rsiSetBrush(rsiCONTEXT*, void*, ...);
rsiResult stub_rsiSetHyperTexture(rsiCONTEXT*, void*, ...);
rsiResult stub_rsiSetActor(rsiCONTEXT*, void*, ...);
rsiResult stub_rsiPosition(rsiCONTEXT*, void*, float, float, rsiVECTOR*, ...);
rsiResult stub_rsiAlignment(rsiCONTEXT*, void *, float, float, rsiVECTOR*, ...);
rsiResult stub_rsiSize(rsiCONTEXT*, void*, float, float, rsiVECTOR*, ...);

rsiResult stub_rsiSetCooperateCB(rsiCONTEXT*, void (*)(void*));
rsiResult stub_rsiSetCheckCancelCB(rsiCONTEXT*, BOOL (*)(void*));
rsiResult stub_rsiSetLogCB(rsiCONTEXT*, void (*)(void*, char *));
rsiResult stub_rsiSetUpdateStatusCB(rsiCONTEXT*, void (*)(void*, float, float, int, int, rsiSMALL_COLOR*));
rsiResult stub_rsiCreateContext(rsiCONTEXT**);
rsiResult stub_rsiFreeContext(rsiCONTEXT*);
rsiResult stub_rsiSetUserData(rsiCONTEXT*, void*);
rsiResult stub_rsiGetUserData(rsiCONTEXT*, void**);
rsiResult stub_rsiGetErrorMsg(rsiCONTEXT*, char *, int);
rsiResult stub_rsiCleanup(rsiCONTEXT*);
//rsiResult stub_rsiGetStats(rsiSTAT **, int *);
//rsiResult stub_rsiGetStatDescription(rsiSTAT *, char *, int);
rsiResult stub_rsiSetOctreeDepth(rsiCONTEXT*, int);
rsiResult stub_rsiSetRenderField(rsiCONTEXT*, int, int, int, int);
rsiResult stub_rsiSetDistribution(rsiCONTEXT*, int, int);
rsiResult stub_rsiSetRandomJitter(rsiCONTEXT*, BOOL);
rsiResult stub_rsiSetScreen(rsiCONTEXT*, int, int);
rsiResult stub_rsiSetBrushPath(rsiCONTEXT*, char*);
rsiResult stub_rsiSetTexturePath(rsiCONTEXT*, char*);
rsiResult stub_rsiSetObjectPath(rsiCONTEXT*, char*);
rsiResult stub_rsiCreatePointLight(rsiCONTEXT*, void **);
rsiResult stub_rsiCreateSpotLight(rsiCONTEXT*, void **);
rsiResult stub_rsiCreateDirectionalLight(rsiCONTEXT*, void **);
rsiResult stub_rsiCreateFlare(rsiCONTEXT*, void *, void *);
rsiResult stub_rsiCreateStar(rsiCONTEXT*, void *, void *);
rsiResult stub_rsiCreateSphere(rsiCONTEXT*, void **, void *);
rsiResult stub_rsiCreateBox(rsiCONTEXT*, void **, void *);
rsiResult stub_rsiCreateCylinder(rsiCONTEXT*, void **, void *);
rsiResult stub_rsiCreateSOR(rsiCONTEXT*, void **, void *, int, rsiVECTOR2D*);
rsiResult stub_rsiCreateCone(rsiCONTEXT*, void **, void *);
rsiResult stub_rsiCreatePlane(rsiCONTEXT*, void **,void *);
rsiResult stub_rsiCreateCSG(rsiCONTEXT*, void **);
rsiResult stub_rsiAddImagineTexture(rsiCONTEXT*, void **, char *, void *);
rsiResult stub_rsiAddRayStormTexture(rsiCONTEXT*, void **, char *, void *);
rsiResult stub_rsiAddBrush(rsiCONTEXT*, void **, char *, void *);
rsiResult stub_rsiAddHyperTexture(rsiCONTEXT*, void **, int, void *);
rsiResult stub_rsiCreateActor(rsiCONTEXT*, void **);

#endif // PPC_RSI_STUBS_H
