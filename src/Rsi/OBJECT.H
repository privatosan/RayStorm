/***************
 * MODUL:         object
 * NAME:          object.h
 * DESCRIPTION:   object baseclass definition
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    11.02.95 ah    initial release
 *    27.08.95 ah    added actor, UpdateObjects, GenVoxels
 *    02.09.95 ah    added time
 *    11.10.95 ah    added Identify() and stuff for it
 *    13.12.96 ah    added minobjects to BuildOctree()
 *    10.01.97 ah    added IDENT_BOX
 *    11.01.97 ah    added IDENT_CSG
 *    12.01.97 ah    added Remove()
 *    18.03.97 ah    added IDENT_CYLINDER
 *    20.03.97 ah    added IDENT_SOR
 *    23.03.97 ah    added IDENT_CONE
 *    09.07.97 ah    changed type of flags from BOOL to UWORD (should save us 2 bytes per object in PC version)
 *    09.07.97 ah    changed type of flags from UWORD to ULONG (long word access is faster)
 *    10.04.99 ah    added BrushCoords()
 ***************/

#ifndef OBJECT_H
#define OBJECT_H

class OBJECT;

#ifndef VOXEL_H
#include "voxel.h"
#endif

#ifndef INTERSEC_H
#include "intersec.h"
#endif

#ifndef LIST_H
#include "list.h"
#endif

#ifndef COLOR_H
#include "color.h"
#endif

#ifndef WORLD_H
#include "world.h"
#endif

#ifndef SURFACE_H
#include "surface.h"
#endif

#ifndef BRUSH_H
#include "brush.h"
#endif

#ifndef CAMERA_H
#include "camera.h"
#endif

#ifndef ACTOR_H
#include "actor.h"
#endif

#ifndef OCTREE_H
#include "octree.h"
#endif

#define OBJECT_UNLIMITED   0x0001   // object is unlimited (needed for octree)
#define OBJECT_INVERTED    0x0002   // invert object (needed for CSG)
#define OBJECT_CLOSETOP    0x0004   // close top cap of sor or cylinder
#define OBJECT_CLOSEBOTTOM 0x0008   // close bottom cap of sor or cylinder
#define OBJECT_SORSTURM    0x0010   // use more accurate sturm root solver for sor

#define OBJECT_SIDE        0x0020   // object normal (cone, cylinder, sor)
#define OBJECT_CAPS        0x0040
#define OBJECT_NORMALS1    0x0060

#define OBJECT_XNORMAL     0x0020   // object normal (triangle, box)
#define OBJECT_YNORMAL     0x0040
#define OBJECT_ZNORMAL     0x0080
#define OBJECT_NORMALS     0x00e0

#define OBJECT_FIRSTBLOCK  0x0100   // First triangle of block
#define OBJECT_INBLOCK     0x0200   // Triangle is in block

#define OBJECT_CSG_UNION      0x0100   // CSG operation
#define OBJECT_CSG_INTERSECT  0x0200
#define OBJECT_CSG_DIFFERENCE 0x0300
#define OBJECT_CSG_MASK       0x0300

class OBJECT : public SLIST    // object
{
	public:
		VOXEL voxel;            // surrounding voxel
		ULONG flags;            // flags
		ULONG identification;   // identification
		SURFACE *surf;          // surface definition
		ACTOR *actor;           // pointer to actor
		float time;             // current time

#ifdef __MAXON__
		void *operator new(size_t);
		void operator delete(void*);
#endif

		OBJECT();
		void Insert(RSICONTEXT*);
		virtual ULONG Identify() = 0;
		virtual void Update(const float) = 0;
		virtual void GenVoxel() = 0;
		virtual BOOL RayIntersect(RAY *) = 0;
		virtual void Normal(const VECTOR *, const VECTOR *, VECTOR *, VECTOR *) = 0;
		void GetColor(RSICONTEXT*, RAY*, COLOR*, COLOR*, SMALL_COLOR*, int*, VECTOR *pre_norm=NULL);
		virtual float Density(const VECTOR*) { return 1.; };
		virtual void DisableObject(RSICONTEXT*);
		virtual void BrushCoords(const VECTOR *, float*, float*) { };
};

enum
{
	IDENT_TRIANGLE,
	IDENT_SPHERE,
	IDENT_PLANE,
	IDENT_BOX,
	IDENT_CSG,
	IDENT_CYLINDER,
	IDENT_SOR,
	IDENT_CONE
};

OBJECT *IntersectRayOctree(RSICONTEXT*, RAY *, int *, int mode);
void SampleLine(RSICONTEXT*, int, SMALL_COLOR *, COLOR *, float);
void SamplePixel(RSICONTEXT*, int, int, float, float, SMALL_COLOR*);
void CreateFilter(float *, WORLD *);
void SuperSampleLine(RSICONTEXT*, int, SMALL_COLOR*, const float*, COLOR*);
void SuperSamplePixel(RSICONTEXT*, int, int, SMALL_COLOR *, const float *, COLOR*, float);
void AdaptiveRefineLine(RSICONTEXT*, int , SMALL_COLOR *, SMALL_COLOR *,
	SMALL_COLOR *, const float *, COLOR*, COLOR*, COLOR*);
void UpdateObjects(RSICONTEXT*, const float);
void GenVoxels(RSICONTEXT*);

#endif /* OBJECT_H */
