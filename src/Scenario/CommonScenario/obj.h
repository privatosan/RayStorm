/***************
 * MODUL:         Object handler
 * NAME:          objlink.h
 * DESCRIPTION:   definition of object handler link structure
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    29.01.96 ah    initial release
 *    18.04.96 ah    added surf to read()
 *    29.11.96 ah    added flare and star functions
 *    25.01.97 ah    added AddBox()
 *    31.01.97 ah    added Star_Tilt()
 *    06.03.97 ah    added PointLight_ShadowMapSize() and SpotLight_ShadowMapSize()
 *    07.03.97 ah    added AddMesh()
 *    26.03.97 ah    added AddCylinder()
 *    26.03.97 ah    added AddCone()
 *    16.05.97 ah    added SetName(), GetSurfaceByName()
 *    09.06.97 ah    added handling to V2 style
 *    17.07.97 ah    added ObjectFlags() and ObjectTrack()
 *    25.09.97 ah    added SOR functions
 ***************/

#ifndef OBJLINK_H
#define OBJLINK_H

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef TRIANGLE_H
#include "triangle.h"
#endif

enum
{
	LINK_RENDERER, LINK_SCENARIO
};

typedef struct
{
	UWORD p[2];    // point numbers
	UWORD flags;
} LINK_EDGE;

#define LINK_EDGE_SHARP 1

typedef struct
{
	UWORD e[3];    // edge numbers
} LINK_MESH;

typedef struct tOBJLINK
{
	int         type;

	void        (*ObjectBegin)(rsiCONTEXT*);
	void        (*ObjectEnd)(rsiCONTEXT*);
	void        (*ObjectSurface)(rsiCONTEXT*, OBJECT*, SURFACE*);
	BOOL        (*ObjectName)(rsiCONTEXT*, OBJECT*, char*);
	void        (*ObjectAxis)(rsiCONTEXT*, OBJECT*, VECTOR *,VECTOR *,VECTOR *,VECTOR *,VECTOR *);
	void        (*ObjectActor)(rsiCONTEXT*, OBJECT*, ACTOR*);
	void        (*ObjectFlags)(rsiCONTEXT*, OBJECT*, ULONG);
	BOOL        (*ObjectTrack)(rsiCONTEXT*, OBJECT*, char*);

	SURFACE*    (*SurfaceAdd)(rsiCONTEXT*);
	SURFACE*    (*SurfaceGetByName)(rsiCONTEXT*, char*);
	BOOL        (*SurfaceName)(rsiCONTEXT*, SURFACE *,char *);
	void        (*SurfaceSetFlags)(rsiCONTEXT*, SURFACE *,UWORD);
	void        (*SurfaceDiffuse)(rsiCONTEXT*, SURFACE *,float,float,float);
	void        (*SurfaceSpecular)(rsiCONTEXT*, SURFACE *,float,float,float);
	void        (*SurfaceAmbient)(rsiCONTEXT*, SURFACE *,float,float,float);
	void        (*SurfaceReflect)(rsiCONTEXT*, SURFACE *,float,float,float);
	void        (*SurfaceTranspar)(rsiCONTEXT*, SURFACE *,float,float,float);
	void        (*SurfaceDiffTrans)(rsiCONTEXT*, SURFACE *,float,float,float);
	void        (*SurfaceSpecTrans)(rsiCONTEXT*, SURFACE *,float,float,float);
	void        (*SurfaceFoglen)(rsiCONTEXT*, SURFACE *,float);
	float       (*SurfaceGetFoglen)(rsiCONTEXT*, SURFACE *);
	void        (*SurfaceIndOfRefr)(rsiCONTEXT*, SURFACE *,float);
	void        (*SurfaceRefPhong)(rsiCONTEXT*, SURFACE *,float);
	void        (*SurfaceTransPhong)(rsiCONTEXT*, SURFACE *,float);
	void        (*SurfaceTransluc)(rsiCONTEXT*, SURFACE *,float);
	BOOL        (*SurfaceCopy)(rsiCONTEXT*, SURFACE *,SURFACE *);

	OBJECT*     (*SphereAdd)(rsiCONTEXT*);
	void        (*SphereAxis)(rsiCONTEXT*, OBJECT*, VECTOR*, float);

	TRIANGLE*   (*TriangleAdd)(rsiCONTEXT*, int,SURFACE *,ACTOR *);
	BOOL        (*TriangleAddPoints)(rsiCONTEXT*, int,VECTOR *);
	BOOL        (*TriangleAddEdges)(rsiCONTEXT*, int,UWORD*);
	void        (*TriangleSetEdges)(rsiCONTEXT*, TRIANGLE *,UWORD,UWORD,UWORD);
	void        (*TriangleSetEdgeFlags)(rsiCONTEXT*, UBYTE *);
	void        (*TrianglePoints)(rsiCONTEXT*, TRIANGLE *,VECTOR *,VECTOR *,VECTOR *);
	BOOL        (*TriangleVNorm)(rsiCONTEXT*, TRIANGLE *,VECTOR *,VECTOR *,VECTOR *);
	void        (*TriangleSurface)(rsiCONTEXT*, TRIANGLE*, SURFACE*);
	TRIANGLE*   (*TriangleGetNext)(rsiCONTEXT*, TRIANGLE *);

	OBJECT*     (*MeshCreate)(rsiCONTEXT*);
	BOOL        (*MeshAddScenario)(rsiCONTEXT*, OBJECT*, VECTOR*, int, LINK_EDGE*, int, LINK_MESH*, int, BOOL);
	OBJECT*     (*MeshAddRenderer)(rsiCONTEXT*, SURFACE*, SURFACE**, ACTOR*, VECTOR*, int, LINK_EDGE*, int, LINK_MESH*, int, VECTOR*, VECTOR*, VECTOR*, VECTOR*, VECTOR*, VECTOR*, VECTOR*, VECTOR*, VECTOR*, VECTOR*, BOOL);

	OBJECT*     (*BoxAdd)(rsiCONTEXT*);
	void        (*BoxBounds)(rsiCONTEXT*, OBJECT*, VECTOR*, VECTOR*);
	void        (*BoxAxis)(rsiCONTEXT*, OBJECT*, VECTOR*, VECTOR*, VECTOR*, VECTOR*);

	OBJECT*     (*CylinderAdd)(rsiCONTEXT*);
	void        (*CylinderAxis)(rsiCONTEXT*, OBJECT *, VECTOR *, VECTOR *, VECTOR *, VECTOR *, VECTOR *);
	void        (*CylinderFlags)(rsiCONTEXT*, OBJECT *, ULONG);

	OBJECT*     (*ConeAdd)(rsiCONTEXT*);
	void        (*ConeAxis)(rsiCONTEXT*, OBJECT*, VECTOR *, VECTOR *, VECTOR *, VECTOR *, VECTOR *);
	void        (*ConeFlags)(rsiCONTEXT*, OBJECT *, ULONG);

	OBJECT*     (*PlaneAdd)(rsiCONTEXT*);
	void        (*PlaneAxis)(rsiCONTEXT*, OBJECT*, VECTOR*, VECTOR*);

	OBJECT*     (*CSGAdd)(rsiCONTEXT*);
	void        (*CSGOperation)(rsiCONTEXT*, OBJECT*, UWORD);

	OBJECT*     (*SORAdd)(rsiCONTEXT*);
	void        (*SORAxis)(rsiCONTEXT*, OBJECT*, VECTOR *, VECTOR *, VECTOR *, VECTOR *, VECTOR *);
	void        (*SORFlags)(rsiCONTEXT*, OBJECT *, ULONG);
	BOOL        (*SORPoints)(rsiCONTEXT*, OBJECT*, VECT2D*, ULONG);

	IM_TEXTURE* (*TextureAdd)(rsiCONTEXT*, char *,SURFACE *);
	TEXTURE_OBJECT*(*TextureObjectAdd)(rsiCONTEXT*, OBJECT*, ACTOR*, TEXTURE_OBJECT*);
	void        (*TextureParams)(rsiCONTEXT*, IM_TEXTURE *,int,float);
	void        (*TexturePos)(rsiCONTEXT*, TEXTURE_OBJECT *,VECTOR *);
	void        (*TextureOrient)(rsiCONTEXT*, TEXTURE_OBJECT *,VECTOR *,VECTOR *,VECTOR *);
	void        (*TextureSize)(rsiCONTEXT*, TEXTURE_OBJECT *,VECTOR *);

	BRUSH*      (*BrushAdd)(rsiCONTEXT*, char *,SURFACE *);
	BRUSH_OBJECT*(*BrushObjectAdd)(rsiCONTEXT*, OBJECT*, ACTOR*, BRUSH_OBJECT*);
	void        (*BrushFlags)(rsiCONTEXT*, BRUSH *,ULONG);
	void        (*BrushPos)(rsiCONTEXT*, BRUSH_OBJECT *,VECTOR *);
	void        (*BrushOrient)(rsiCONTEXT*, BRUSH_OBJECT *,VECTOR *,VECTOR *,VECTOR *);
	void        (*BrushSize)(rsiCONTEXT*, BRUSH_OBJECT *,VECTOR *);
	void        (*BrushAlignment)(rsiCONTEXT*, BRUSH_OBJECT *,VECTOR *);

	CAMERA*     (*CameraAdd)(rsiCONTEXT*);
	void        (*CameraFlags)(rsiCONTEXT*, CAMERA*, ULONG);
	void        (*CameraFocalDist)(rsiCONTEXT*, CAMERA*, float);
	void        (*CameraAperture)(rsiCONTEXT*, CAMERA*, float);
	void        (*CameraFOV)(rsiCONTEXT*, CAMERA*, float, float);

	void        (*LightPos)(rsiCONTEXT*, LIGHT*, VECTOR *);
	void        (*LightColor)(rsiCONTEXT*, LIGHT*, float, float, float);
	void        (*LightFlags)(rsiCONTEXT*, LIGHT*, ULONG);
	void        (*LightFallOff)(rsiCONTEXT*, LIGHT*, float);
	void        (*LightRadius)(rsiCONTEXT*, LIGHT*, float);
	void        (*LightShadowMapSize)(rsiCONTEXT*, LIGHT*, int);

	LIGHT*      (*PointLightAdd)(rsiCONTEXT*);

	LIGHT*      (*SpotLightAdd)(rsiCONTEXT*);
	void        (*SpotLightAngle)(rsiCONTEXT*, LIGHT*, float);

	FLARE*      (*FlareAdd)(rsiCONTEXT*, LIGHT *);
	void        (*FlareColor)(rsiCONTEXT*, FLARE*, float, float, float);
	void        (*FlarePosition)(rsiCONTEXT*, FLARE*, float);
	void        (*FlareRadius)(rsiCONTEXT*, FLARE*, float);
	void        (*FlareType)(rsiCONTEXT*, FLARE*, ULONG);
	void        (*FlareFunction)(rsiCONTEXT*, FLARE*, ULONG);
	void        (*FlareEdges)(rsiCONTEXT*, FLARE*, ULONG);
	void        (*FlareTilt)(rsiCONTEXT*, FLARE*, float);

	STAR*       (*StarAdd)(rsiCONTEXT*, LIGHT*);
	void        (*StarFlags)(rsiCONTEXT*, STAR*, ULONG);
	void        (*StarRadius)(rsiCONTEXT*, STAR*, float);
	void        (*StarTilt)(rsiCONTEXT*, STAR*, float);
	void        (*StarSpikes)(rsiCONTEXT*, STAR*, ULONG);
	void        (*StarHaloradius)(rsiCONTEXT*, STAR*, float);
	void        (*StarInnerHaloradius)(rsiCONTEXT*, STAR*, float);
	void        (*StarRange)(rsiCONTEXT*, STAR*, float);
	BOOL        (*StarIntensities)(rsiCONTEXT*, STAR*, int, float*);
} OBJLINK;

// flags
#define LINK_OBJECT_NODEOPEN        0x0001
#define LINK_OBJECT_INVERTED        0x0002

#define LINK_CYLINDER_OPENTOP       0x0001
#define LINK_CYLINDER_OPENBOTTOM    0x0002

#define LINK_CONE_OPENBOTTOM        0x0001

#define LINK_CSG_UNION              0x0001
#define LINK_CSG_INTERSECTION       0x0002
#define LINK_CSG_DIFFERENCE         0x0003

#define LINK_SOR_OPENTOP            0x0001
#define LINK_SOR_OPENBOTTOM         0x0002
#define LINK_SOR_ACCURATE           0x0004

#define LINK_LIGHT_SHADOW           0x0001
#define LINK_LIGHT_FLARES           0x0002
#define LINK_LIGHT_STAR             0x0004
#define LINK_LIGHT_SHADOWMAP        0x0008
#define LINK_LIGHT_TRACKFALLOFF     0x0010

#define LINK_CAMERA_VFOV            0x0001
#define LINK_CAMERA_FOCUSTRACK      0x0002
#define LINK_CAMERA_FASTDOF         0x0004

#define LINK_MESH_NOPHONG           0x0001
#define LINK_MESH_EXTERNAL_APPLY    0x0002

#define LINK_BRUSH_WRAPX            0x0001
#define LINK_BRUSH_WRAPY            0x0002
#define LINK_BRUSH_WRAPXY           0x0003
#define LINK_BRUSH_WRAPMASK         0x0003
#define LINK_BRUSH_MAP_COLOR        0x0008
#define LINK_BRUSH_MAP_REFLECTIFITY 0x0010
#define LINK_BRUSH_MAP_FILTER       0x0020
#define LINK_BRUSH_MAP_ALTITUDE     0x0030
#define LINK_BRUSH_MAP_SPECULAR     0x0040
#define LINK_BRUSH_MAP_REFLECTION   0x0080
#define LINK_BRUSH_MAP_MASK         0x00f0
#define LINK_BRUSH_SOFT             0x0100
#define LINK_BRUSH_REPEAT           0x0200
#define LINK_BRUSH_MIRROR           0x0400

#define LINK_SURFACE_BRIGHT         0x0001

#define LINK_STAR_ENABLE            0x0001
#define LINK_STAR_HALOENABLE        0x0002
#define LINK_STAR_RANDOM            0x0004

#endif // OBJLINK_H
