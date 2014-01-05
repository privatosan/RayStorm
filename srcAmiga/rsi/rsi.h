 /***************
 * MODULE:     rsi.h
 * PROJECT:    RayStorm
 * DESCRIPTION:interface for RayStorm (tm) renderer, header file
 * AUTHORS:    Andreas Heumann & Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    12.02.95 mh    transfer of methods from raystorm.cpp
 *    23.05.96 mh    rsi standard
 *    08.10.96 ah    added 'Orient'-parameters for brushes and textures
 *    22.10.96 mh    added rsiAddFlare
 *    27.11.96 ah    added rsiTStarInnerHaloRadius
 *    05.12.96 ah    added statistic functions
 *    14.12.96 ah    added rsiTWorldMinObjects and rsiTStarSpikeWidth
 *    10.01.97 ah    added values for box object
 *    12.01.97 ah    added values for CSG object
 *    04.03.97 ah    added rsiFLightShadowMap and rsiTLightShadowMapSize
 *    07.03.97 ah    added rsiCreateMesh, rsiTMeshActor and rsiTMeshNoPhong
 *    12.03.97 ah    added hyper texture stuff
 *    18.03.97 ah    added rsiTSphereFlags, rsiTCSGFlags and rsiTBoxFlags
 *    19.03.97 ah    added values for cylinder object
 *    21.03.97 ah    added values for sor object
 *    23.03.97 ah    added values for cone object
 *    03.11.97 ah    added rsiPicJPEGQuality
 *    18.08.98 ah    added directional light
 *    10.05.99 ah    added brush mapping coordinates and new brush wrap mode
 ***************/

#ifndef RSI_H
#define RSI_H

// camera flags
#define rsiFCameraFastDOF  1

// light flags
#define rsiFLightShadow    1
#define rsiFLightFlares    2
#define rsiFLightStar      4
#define rsiFLightShadowMap 8

// flare flags
#define rsiFFlareDisc      0
#define rsiFFlarePoly      1
#define rsiFFlarePlain     0
#define rsiFFlareExp       1
#define rsiFFlareRadial    2
#define rsiFFlareExp1      3
#define rsiFFlareExp2      4

// sphere flags
#define rsiFSphereInverted 1

// box flags
#define rsiFBoxInverted    1

// cylinder flags
#define rsiFCylinderInverted     1
#define rsiFCylinderOpenTop      2
#define rsiFCylinderOpenBottom   4

// surface of revolution flags
#define rsiFSORInverted          1
#define rsiFSOROpenTop           2
#define rsiFSOROpenBottom        4
#define rsiFSORSturm             8

// cone flags
#define rsiFConeInverted          1
#define rsiFConeOpenBottom        2

// render flags
#define rsiFRenderQuick          1
#define rsiFRenderRandomJitter   2

// surface flags
#define rsiFSurfBright     1

// brush flags
#define rsiFBrushColor        0x000000
#define rsiFBrushReflectivity 0x000001
#define rsiFBrushFilter       0x000002
#define rsiFBrushAltitude     0x000003
#define rsiFBrushReflection   0x000004
#define rsiFBrushSpecular     0x000005
#define rsiFBrushSoft         0x000100    // Soft color interpolation
#define rsiFBrushWrapX        0x010000
#define rsiFBrushWrapY        0x020000
#define rsiFBrushWrapXY       0x030000
#define rsiFBrushWrapCoord    0x040000
#define rsiFBrushRepeat       0x080000
#define rsiFBrushMirror       0x100000

// actor flag
#define rsiFActionLinear      1     // linear interpolation
#define rsiFActionSpline      2     // spline interpolation

// CSG fags
#define rsiFCSGUnion          0
#define rsiFCSGIntersection   1
#define rsiFCSGDifference     2
#define rsiFCSGInverted       1

// hyper textures
enum
{
	rsiFHyperExplosion
};

#define rsiFDefault  -1

#ifndef APTR_TYPEDEF
typedef int BOOL;
typedef unsigned char UBYTE;
#endif

typedef struct
{
	float x, y, z;
} rsiVECTOR;

typedef struct
{
	float x, y;
} rsiVECTOR2D;

typedef struct
{
	float r, g, b;
} rsiCOLOR;

typedef struct
{
	UBYTE r,g,b;
	UBYTE pad;
} rsiSMALL_COLOR;

typedef struct
{
	int type;
	int value;
} rsiSTAT;

typedef struct
{
	UWORD p[2];    // point numbers
	UWORD flags;
} rsiEDGE;

#define EDGE_SHARP 1

typedef struct
{
	UWORD e[3];    // edge numbers
} rsiMESH;

typedef void rsiCONTEXT;

enum rsiSYSTEM
{
	rsiSYSTEM_M68K,
	rsiSYSTEM_PPC_WarpOS
};

typedef struct
{
	rsiSYSTEM system; // system for which rsi is compiled for
} rsiLIBINFO;

/* error codes */
enum
{
	rsiERR_NONE,
	rsiERR_MEM,
	rsiERR_DEMOLIMIT,
	rsiERR_RESOLUTION,
	rsiERR_TRIANGLE,
	rsiERR_VIEW,
	rsiERR_SCREENMEM,
	rsiERR_BACKDROPSIZE,
	rsiERR_OPENTEXTUREFILE,
	rsiERR_OPENBRUSH,
	rsiERR_INITTEXTURE,
	rsiERR_OPENPIC,
	rsiERR_READPIC,
	rsiERR_OPENPICTYPEFILE,
	rsiERR_READPICTYPEFILE,
	rsiERR_UNKNOWNPIC,
	rsiERR_PICHANDLER,
	rsiERR_OPENOBJ,
	rsiERR_READOBJ,
	rsiERR_OPENOBJTYPEFILE,
	rsiERR_READOBJTYPEFILE,
	rsiERR_UNKNOWNOBJ,
	rsiERR_OBJHANDLER,
	rsiERR_ACTOR,
	rsiERR_SURFACE,
	rsiERR_OCTREEDEPTH,
	rsiERR_ANTIALIAS,
	rsiERR_TIME,
	rsiERR_NOPIC,
	rsiERR_DISTRIB,
	rsiERR_WRONGERROR,
	rsiERR_PARAM,
	rsiERR_STATNAME,
	rsiERR_SORNUMBER,
	rsiERR_SORPOINT,
	rsiERR_CSG,
	rsiERR_NOTCSGABLE,
	rsiERR_WRONGOBJHANDLER,
	rsiERR_WRONGPICHANDLER,
	rsiERR_EXTERNAL
};

typedef int rsiResult;

#ifndef LIBRARY

#ifdef __cplusplus
extern "C"
{
#endif

/* set callback functions */
rsiResult rsiSetCooperateCB(rsiCONTEXT*, void (*)(void*));
rsiResult rsiSetCheckCancelCB(rsiCONTEXT*, BOOL (*)(void*));
rsiResult rsiSetLogCB(rsiCONTEXT*, void (*)(void*, char *));
rsiResult rsiSetUpdateStatusCB(rsiCONTEXT*, void (*)(void*, float, float, int, int, rsiSMALL_COLOR*));

/* general commands */
rsiResult rsiGetLibInfo(rsiLIBINFO*);
rsiResult rsiInit();
rsiResult rsiExit();
rsiResult rsiCreateContext(rsiCONTEXT**);
rsiResult rsiFreeContext(rsiCONTEXT*);
rsiResult rsiSetUserData(rsiCONTEXT*, void*);
rsiResult rsiGetUserData(rsiCONTEXT*, void**);
rsiResult rsiGetErrorMsg(rsiCONTEXT*, char *, int);
rsiResult rsiCleanup(rsiCONTEXT*);
rsiResult rsiGetStats(rsiSTAT **, int *);
rsiResult rsiGetStatDescription(rsiSTAT *, char *, int);

/* raytracing controling commands */
rsiResult rsiRender(rsiCONTEXT*, UBYTE**, ...);
rsiResult rsiSetOctreeDepth(rsiCONTEXT*, int);
rsiResult rsiSetRenderField(rsiCONTEXT*, int, int, int, int);
rsiResult rsiSetAntialias(rsiCONTEXT*, ...);
rsiResult rsiSetDistribution(rsiCONTEXT*, int, int);
rsiResult rsiSetRandomJitter(rsiCONTEXT*, BOOL);
rsiResult rsiSetWorld(rsiCONTEXT*, ...);
rsiResult rsiSetCamera(rsiCONTEXT*, ...);
rsiResult rsiSetScreen(rsiCONTEXT*, int, int);
rsiResult rsiSetBrushPath(rsiCONTEXT*, char*);
rsiResult rsiSetTexturePath(rsiCONTEXT*, char*);
rsiResult rsiSetObjectPath(rsiCONTEXT*, char*);
rsiResult rsiSavePicture(rsiCONTEXT*, char*, ...);

/* commands for creating objects */
rsiResult rsiCreatePointLight(rsiCONTEXT*, void **);
rsiResult rsiSetPointLight(rsiCONTEXT*, void *, ...);
rsiResult rsiCreateSpotLight(rsiCONTEXT*, void **);
rsiResult rsiSetSpotLight(rsiCONTEXT*, void *, ...);
rsiResult rsiCreateDirectionalLight(rsiCONTEXT*, void **);
rsiResult rsiSetDirectionalLight(rsiCONTEXT*, void *, ...);
rsiResult rsiCreateFlare(rsiCONTEXT*, void *, void *);
rsiResult rsiSetFlare(rsiCONTEXT*, void *, ...);
rsiResult rsiCreateStar(rsiCONTEXT*, void *, void *);
rsiResult rsiSetStar(rsiCONTEXT*, void *, ...);
rsiResult rsiCreateSphere(rsiCONTEXT*, void **, void *);
rsiResult rsiSetSphere(rsiCONTEXT*, void *, ...);
rsiResult rsiCreateBox(rsiCONTEXT*, void **, void *);
rsiResult rsiSetBox(rsiCONTEXT*, void *, ...);
rsiResult rsiCreateCylinder(rsiCONTEXT*, void **, void *);
rsiResult rsiSetCylinder(rsiCONTEXT*, void *, ...);
rsiResult rsiCreateSOR(rsiCONTEXT*, void **, void *, int, rsiVECTOR2D*);
rsiResult rsiSetSOR(rsiCONTEXT*, void *, ...);
rsiResult rsiCreateCone(rsiCONTEXT*, void **, void *);
rsiResult rsiSetCone(rsiCONTEXT*, void *, ...);
rsiResult rsiCreateTriangle(rsiCONTEXT*, void *, rsiVECTOR *, rsiVECTOR *, rsiVECTOR *, ...);
rsiResult rsiCreateMesh(rsiCONTEXT*, void *, rsiVECTOR *, rsiEDGE *, int, rsiMESH *, int, ...);
rsiResult rsiCreatePlane(rsiCONTEXT*, void **,void *);
rsiResult rsiSetPlane(rsiCONTEXT*, void *, ...);
rsiResult rsiCreateCSG(rsiCONTEXT*, void **);
rsiResult rsiSetCSG(rsiCONTEXT*, void *, ...);
rsiResult rsiLoadObject(rsiCONTEXT*, char *, ...);

/* SURFACE relating commands */
rsiResult rsiCreateSurface(rsiCONTEXT*, void **, ...);
rsiResult rsiSetSurface(rsiCONTEXT*, void *, ...);
rsiResult rsiAddImagineTexture(rsiCONTEXT*, void **, char *, void *);
rsiResult rsiAddRayStormTexture(rsiCONTEXT*, void **, char *, void *);
rsiResult rsiSetTexture(rsiCONTEXT*, void *, ...);
rsiResult rsiAddBrush(rsiCONTEXT*, void **, char *, void *);
rsiResult rsiSetBrush(rsiCONTEXT*, void *, ...);
rsiResult rsiAddHyperTexture(rsiCONTEXT*, void **, int, void *);
rsiResult rsiSetHyperTexture(rsiCONTEXT*, void *, ...);

/* ACTOR relating commands */
rsiResult rsiCreateActor(rsiCONTEXT*, void **);
rsiResult rsiSetActor(rsiCONTEXT*, void *, ...);
rsiResult rsiPosition(rsiCONTEXT*, void *, float, float, rsiVECTOR *, ...);
rsiResult rsiAlignment(rsiCONTEXT*, void *, float, float, rsiVECTOR *, ...);
rsiResult rsiSize(rsiCONTEXT*, void *, float, float, rsiVECTOR *, ...);

#ifdef __cplusplus
}
#endif

#endif

/* Argumenttags */
enum tags
{
	rsiTDone,
	rsiTMore,
	rsiTRenderFlags,
	rsiTRenderTimeBegin,
	rsiTRenderTimeEnd,
	rsiTAntiSamples,
	rsiTAntiFilter,
	rsiTAntiContrast,
	rsiTWorldBackground,
	rsiTWorldAmbient,
	rsiTWorldFogLength,
	rsiTWorldFogHeight,
	rsiTWorldFogColor,
	rsiTWorldBackdropPic,
	rsiTWorldReflectionMap,
	rsiTWorldMinObjects,
	rsiTWorldHyperStep,
	rsiTCameraPos,
	rsiTCameraLook,
	rsiTCameraViewUp,
	rsiTCameraPosActor,
	rsiTCameraLookPActor,
	rsiTCameraLookActor,
	rsiTCameraHFov,
	rsiTCameraVFov,
	rsiTCameraFocalDist,
	rsiTCameraAperture,
	rsiTCameraFlags,
	rsiTPicType,
	rsiTPicJPEGQuality,
	rsiTLightPos,
	rsiTLightDir,
	rsiTLightColor,
	rsiTLightFlags,
	rsiTLightRadius,
	rsiTLightActor,
	rsiTLightFallOff,
	rsiTLightLookPoint,
	rsiTLightAngle,
	rsiTLightLookPActor,
	rsiTLightShadowMapSize,
	rsiTLightFallOffRadius,
	rsiTSpherePos,
	rsiTSphereRadius,
	rsiTSphereFuzzy,
	rsiTSphereActor,
	rsiTSphereFlags,
	rsiTTriangleActor,
	rsiTTriangleNorm1,
	rsiTTriangleNorm2,
	rsiTTriangleNorm3,
	rsiTTriangleBrushCoord1,
	rsiTTriangleBrushCoord2,
	rsiTTriangleBrushCoord3,
	rsiTMeshActor,
	rsiTMeshPos,
	rsiTMeshOrientX,
	rsiTMeshOrientY,
	rsiTMeshOrientZ,
	rsiTMeshSize,
	rsiTMeshNoPhong,
	rsiTMeshBrushCoords,
	rsiTPlanePos,
	rsiTPlaneNorm,
	rsiTPlaneActor,
	rsiTObjPos,
	rsiTObjAlign,
	rsiTObjScale,
	rsiTObjActor,
	rsiTObjSurface,
	rsiTObjOrientX,
	rsiTObjOrientY,
	rsiTObjOrientZ,
	rsiTSurfFlags,
	rsiTSurfAmbient,
	rsiTSurfDiffuse,
	rsiTSurfFoglen,
	rsiTSurfSpecular,
	rsiTSurfDifftrans,
	rsiTSurfSpectrans,
	rsiTSurfRefexp,
	rsiTSurfTransexp,
	rsiTSurfRefrindex,
	rsiTSurfReflect,
	rsiTSurfTranspar,
	rsiTSurfTransluc,
	rsiTTextureParams,
	rsiTTexturePara1,
	rsiTTexturePara2,
	rsiTTexturePara3,
	rsiTTexturePara4,
	rsiTTexturePara5,
	rsiTTexturePara6,
	rsiTTexturePara7,
	rsiTTexturePara8,
	rsiTTexturePara9,
	rsiTTexturePara10,
	rsiTTexturePara11,
	rsiTTexturePara12,
	rsiTTexturePara13,
	rsiTTexturePara14,
	rsiTTexturePara15,
	rsiTTexturePara16,
	rsiTTexturePos,
	rsiTTextureAlign,
	rsiTTextureSize,
	rsiTTextureActor,
	rsiTTextureOrientX,
	rsiTTextureOrientY,
	rsiTTextureOrientZ,
	rsiTBrushFlags,
	rsiTBrushPos,
	rsiTBrushAlign,
	rsiTBrushSize,
	rsiTBrushActor,
	rsiTBrushOrientX,
	rsiTBrushOrientY,
	rsiTBrushOrientZ,
	rsiTActorPos,
	rsiTActorAlign,
	rsiTActorSize,
	rsiTPosFlags,
	rsiTAlignFlags,
	rsiTSizeFlags,
	rsiTFlarePos,
	rsiTFlareRadius,
	rsiTFlareColor,
	rsiTFlareType,
	rsiTFlareFunc,
	rsiTFlareEdges,
	rsiTFlareTilt,
	rsiTStarFlags,
	rsiTStarColor,
	rsiTStarRingColor,
	rsiTStarRadius,
	rsiTStarSpikes,
	rsiTStarHaloRadius,
	rsiTStarInnerHaloRadius,
	rsiTStarIntensities,
	rsiTStarBrightness,
	rsiTStarNoise,
	rsiTStarTilt,
	rsiTBoxPos,
	rsiTBoxLowBounds,
	rsiTBoxHighBounds,
	rsiTBoxAlign,
	rsiTBoxOrientX,
	rsiTBoxOrientY,
	rsiTBoxOrientZ,
	rsiTBoxActor,
	rsiTBoxFlags,
	rsiTCSGOperator,
	rsiTCSGObject1,
	rsiTCSGObject2,
	rsiTCSGFlags,
	rsiTHyperPos,
	rsiTHyperAlign,
	rsiTHyperSize,
	rsiTHyperOrientX,
	rsiTHyperOrientY,
	rsiTHyperOrientZ,
	rsiTHyperActor,
	rsiTHyperP1,
	rsiTHyperP2,
	rsiTHyperP3,
	rsiTCylinderPos,
	rsiTCylinderSize,
	rsiTCylinderAlign,
	rsiTCylinderOrientX,
	rsiTCylinderOrientY,
	rsiTCylinderOrientZ,
	rsiTCylinderActor,
	rsiTCylinderFlags,
	rsiTSORPos,
	rsiTSORSize,
	rsiTSORAlign,
	rsiTSOROrientX,
	rsiTSOROrientY,
	rsiTSOROrientZ,
	rsiTSORActor,
	rsiTSORFlags,
	rsiTConePos,
	rsiTConeSize,
	rsiTConeAlign,
	rsiTConeOrientX,
	rsiTConeOrientY,
	rsiTConeOrientZ,
	rsiTConeActor,
	rsiTConeFlags
};

#endif
