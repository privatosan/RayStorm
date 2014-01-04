/***************
 * MODUL:         Object handler
 * NAME:          objhand.cpp
 * DESCRIPTION:   In this module are all the functions to read
 *    object files
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * BUGS:          none
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    29.01.96 ah    initial release
 *    18.04.96 ah    added surf to ReadObject()
 *    28.10.96 ah    added addition functions for RSOB-files
 *    29.11.96 ah    added comments and STAR and FLARE functions
 *    25.01.97 ah    added AddBox()
 *    02.02.97 ah    added Star_Tilt()
 *    06.03.97 ah    added PointLight_ShadowMapSize() and SpotLight_ShadowMapSize()
 *    07.03.97 ah    added AddMesh()
 *    25.03.97 ah    added AddCylinder()
 *    25.03.97 ah    added AddCone()
 *    11.06.97 ah    changed to new V2 handling
 *    25.09.97 ah    added SOR functions
 *    10.05.99 ah    added brush mapping coordinate function (TriangleMapping())
 ***************/

#include <string.h>

#ifndef OBJHAND_H
#include "objhand.h"
#endif

#ifndef BRUSH_H
#include "brush.h"
#endif

#ifndef TEXTURE_H
#include "texture.h"
#endif

#ifndef FILEUTIL_H
#include "fileutil.h"
#endif

#ifndef ERROR_H
#include "error.h"
#endif

#ifndef TRIANGLE_H
#include "triangle.h"
#endif

#ifndef SPHERE_H
#include "sphere.h"
#endif

#ifndef BOX_H
#include "box.h"
#endif

#ifndef CYLINDER_H
#include "cylinder.h"
#endif

#ifndef CONE_H
#include "cone.h"
#endif

#ifndef PLANE_H
#include "plane.h"
#endif

#ifndef CSG_H
#include "csg.h"
#endif

#ifndef SOR_H
#include "sor.h"
#endif

#ifndef LIGHT_H
#include "light.h"
#endif

#ifndef CONTEXT_H
#include "context.h"
#endif

class IM_TEXTURE;
class TEXTURE_OBJECT;
class BRUSH_OBJECT;
typedef RSICONTEXT rsiCONTEXT;

#ifndef OBJLINK_H
#include "objlink.h"
#endif

#ifdef __AMIGA__
	#include <exec/memory.h>
	#ifdef __STORM__
		#include <pragma/dos_lib.h>
		#include <pragma/exec_lib.h>
	#endif
	// StormC assumes that a library base is always global, so lock that
	// library base
	static BOOL LockLibBase = FALSE;
	struct Library *ObjHandBase;
#ifndef DEBUG_LOADER
	#include "rmod:objhand_lib.h"
#else
extern "C"
{
	BOOL objInit();
	char *objRead(RSICONTEXT *rc, char*, OBJLINK*, VECTOR*, VECTOR*, VECTOR*, VECTOR*, VECTOR*, ACTOR *, SURFACE *, ULONG*, float (*)(rsiCONTEXT, float));
	void objCleanup();
}
#endif // !DEBUG_LOADER

#else

typedef BOOL *(*INITPROC)();
typedef char *(*READWRITEPROC)(RSICONTEXT *, char *, OBJLINK *, VECTOR *, VECTOR *, VECTOR *, VECTOR *, VECTOR *, ACTOR *, SURFACE *, ULONG *, float (*)(rsiCONTEXT, float));
typedef VOID *(*CLEANUPPROC)();

extern char WorkingDirectory[256];
#endif // __AMIGA__

#ifdef __PPC__
#define SAVEDS __saveds
#else
#ifndef SAVEDS
#define SAVEDS
#endif
#endif // __PPC__

/*************
 * DESCRIPTION:   called at start of new object
 * INPUT:         -
 * OUTPUT:        -
 *************/
static void SAVEDS ObjectBegin(RSICONTEXT *rc)
{
}

/*************
 * DESCRIPTION:   called at end of an object
 * INPUT:         -
 * OUTPUT:        -
 *************/
static void SAVEDS ObjectEnd(RSICONTEXT *rc)
{
	HIERARCHIE *next;

	if(rc->cur_csg)
	{
		if(rc->cur_node == &rc->cur_csg->obj1)
			rc->cur_node = &rc->cur_csg->obj2;
		else
		{
			if(rc->cur_hier)
			{
				next = rc->cur_hier;
				rc->cur_hier = rc->cur_hier->prev;
				delete next;
				if(rc->cur_hier)
				{
					rc->cur_csg = rc->cur_hier->node;
					rc->cur_node = &rc->cur_csg->obj1;
				}
				else
					rc->cur_csg = NULL;
			}
		}
	}
}

/*************
 * DESCRIPTION:   called to set the surface of the object
 * INPUT:         obj      object
 *                surf     surface
 * OUTPUT:        -
 *************/
static void SAVEDS ObjectSurface(RSICONTEXT *rc, OBJECT *obj, SURFACE *surf)
{
	obj->surf = surf;
}

/*************
 * DESCRIPTION:   called to set the object axis
 * INPUT:         pos      position
 *                orient_x,orient_y,orient_z    orientation
 *                size     size
 * OUTPUT:        -
 *************/
static void SAVEDS ObjectAxis(RSICONTEXT *rc, OBJECT *obj, VECTOR *pos,VECTOR *orient_x,VECTOR *orient_y,
	VECTOR *orient_z,VECTOR *size)
{
}

/*************
 * DESCRIPTION:   called to set the actor of the object
 * INPUT:         obj      object
 *                actor    actor
 * OUTPUT:        -
 *************/
static void SAVEDS ObjectActor(RSICONTEXT *rc, OBJECT *obj, ACTOR *actor)
{
	obj->actor = actor;
}

/*************
 * DESCRIPTION:   called to set the flags of the object
 * INPUT:         obj      object
 *                flags    flags
 * OUTPUT:        -
 *************/
static void SAVEDS ObjectFlags(RSICONTEXT *rc, OBJECT *obj, ULONG flags)
{
	if(flags & LINK_OBJECT_INVERTED)
		obj->flags |= OBJECT_INVERTED;
}

/*************
 * DESCRIPTION:   called to add a new surface
 * INPUT:         -
 * OUTPUT:        created surface or NULL if failed
 *************/
static SURFACE* SAVEDS SurfaceAdd(RSICONTEXT *rc)
{
	SURFACE *surf;

	surf = new SURFACE;
	if(!surf)
		return NULL;
	surf->Insert(rc);
	return surf;
}

/*************
 * DESCRIPTION:   called to get a surface by it's name
 * INPUT:         -
 * OUTPUT:        NULL
 *************/
static SURFACE* SAVEDS SurfaceGetByName(RSICONTEXT *rc, char *name)
{
	SURF_NAME *surf_name;

	surf_name = rc->surfname_root;
	while(surf_name)
	{
		if(!strcmp(surf_name->name, name))
			return surf_name->surf;
		surf_name = (SURF_NAME*)surf_name->GetNext();
	}
	return NULL;
}

/*************
 * DESCRIPTION:   called to set the surface name
 * INPUT:         surf     surface
 *                name     name
 * OUTPUT:        FALSE if failed else TRUE
 *************/
static BOOL SAVEDS SurfaceName(RSICONTEXT *rc, SURFACE *surf,char *name)
{
	SURF_NAME *surf_name;

	surf_name = new SURF_NAME;
	if(!surf_name)
		return FALSE;

	surf_name->name = new char[strlen(name)+1];
	if(!surf_name->name)
	{
		delete surf_name;
		return FALSE;
	}
	strcpy(surf_name->name, name);
	surf_name->surf = surf;

	surf_name->Insert((SLIST**)&rc->surfname_root);

	return TRUE;
}

/*************
 * DESCRIPTION:   called to set surface flags
 * INPUT:         surf     surface
 *                flags    flags
 * OUTPUT:        -
 *************/
static void SAVEDS SurfaceSetFlags(RSICONTEXT *rc, SURFACE *surf,UWORD flags)
{
	if(flags & LINK_SURFACE_BRIGHT)
		surf->flags |= BRIGHT;
}

/*************
 * DESCRIPTION:   called to set surface diffuse color
 * INPUT:         surf     surface
 *                r,g,b    color
 * OUTPUT:        -
 *************/
static void SAVEDS SurfaceDiffuse(RSICONTEXT *rc, SURFACE *surf,float r,float g,float b)
{
	surf->diffuse.r = r;
	surf->diffuse.g = g;
	surf->diffuse.b = b;
}

/*************
 * DESCRIPTION:   called to set surface specular color
 * INPUT:         surf     surface
 *                r,g,b    color
 * OUTPUT:        -
 *************/
static void SAVEDS SurfaceSpecular(RSICONTEXT *rc, SURFACE *surf,float r,float g,float b)
{
	surf->specular.r = r;
	surf->specular.g = g;
	surf->specular.b = b;
}

/*************
 * DESCRIPTION:   called to set surface ambient color
 * INPUT:         surf     surface
 *                r,g,b    color
 * OUTPUT:        -
 *************/
static void SAVEDS SurfaceAmbient(RSICONTEXT *rc, SURFACE *surf,float r,float g,float b)
{
	surf->ambient.r = r;
	surf->ambient.g = g;
	surf->ambient.b = b;
}

/*************
 * DESCRIPTION:   called to set surface reflective color
 * INPUT:         surf     surface
 *                r,g,b    color
 * OUTPUT:        -
 *************/
static void SAVEDS SurfaceReflect(RSICONTEXT *rc, SURFACE *surf,float r,float g,float b)
{
	surf->reflect.r = r;
	surf->reflect.g = g;
	surf->reflect.b = b;
}

/*************
 * DESCRIPTION:   called to set surface transparent color
 * INPUT:         surf     surface
 *                r,g,b    color
 * OUTPUT:        -
 *************/
static void SAVEDS SurfaceTranspar(RSICONTEXT *rc, SURFACE *surf,float r,float g,float b)
{
	surf->transpar.r = r;
	surf->transpar.g = g;
	surf->transpar.b = b;
}

/*************
 * DESCRIPTION:   called to set surface diffuse transmission color
 * INPUT:         surf     surface
 *                r,g,b    color
 * OUTPUT:        -
 *************/
static void SAVEDS SurfaceDiffTrans(RSICONTEXT *rc, SURFACE *surf, float r, float g, float b)
{
	surf->difftrans.r = r;
	surf->difftrans.g = g;
	surf->difftrans.b = b;
}

/*************
 * DESCRIPTION:   called to set surface specular transmission color
 * INPUT:         surf     surface
 *                r,g,b    color
 * OUTPUT:        -
 *************/
static void SAVEDS SurfaceSpecTrans(RSICONTEXT *rc, SURFACE *surf, float r, float g, float b)
{
	surf->spectrans.r = r;
	surf->spectrans.g = g;
	surf->spectrans.b = b;
}

/*************
 * DESCRIPTION:   called to set surface fog length
 * INPUT:         surf     surface
 *                flen     fog length
 * OUTPUT:        -
 *************/
static void SAVEDS SurfaceFoglen(RSICONTEXT *rc, SURFACE *surf,float flen)
{
	surf->foglength = flen;
}

/*************
 * DESCRIPTION:   called to get surface fog length
 * INPUT:         surf     surface
 * OUTPUT:        fog length
 *************/
static float SAVEDS SurfaceGetFoglen(RSICONTEXT *rc, SURFACE *surf)
{
	return surf->foglength;
}

/*************
 * DESCRIPTION:   called to set surface index of refraction
 * INPUT:         surf     surface
 *                ior      index of refraction
 * OUTPUT:        -
 *************/
static void SAVEDS SurfaceIndOfRefr(RSICONTEXT *rc, SURFACE *surf,float ior)
{
	surf->refrindex = ior;
}

/*************
 * DESCRIPTION:   called to set surface reflective phong factor
 * INPUT:         surf     surface
 *                refp     reflective phong
 * OUTPUT:        -
 *************/
static void SAVEDS SurfaceRefPhong(RSICONTEXT *rc, SURFACE *surf,float refp)
{
	surf->refphong = refp;
}

/*************
 * DESCRIPTION:   called to set surface tranparent phong factor
 * INPUT:         surf     surface
 *                transphong  transparent phong
 * OUTPUT:        -
 *************/
static void SAVEDS SurfaceTransPhong(RSICONTEXT *rc, SURFACE *surf, float transphong)
{
	surf->transphong = transphong;
}

/*************
 * DESCRIPTION:   called to set surface translucency factor
 * INPUT:         surf     surface
 *                transluc translucency
 * OUTPUT:        -
 *************/
static void SAVEDS SurfaceTransluc(RSICONTEXT *rc, SURFACE *surf, float transluc)
{
	surf->translucency = transluc;
}

/*************
 * DESCRIPTION:   called to copy a surface
 * INPUT:         from     source
 *                to       destination
 * OUTPUT:        FALSE if failed else TRUE
 *************/
static BOOL SAVEDS SurfaceCopy(RSICONTEXT *rc, SURFACE *from,SURFACE *to)
{
	TEXTURE *texture,*texture_new;
	BRUSH *brush,*brush_new;

	*to = *from;
	to->texture = NULL;
	to->brush = NULL;

	// Copy textures
	texture = (TEXTURE*)from->texture;
	while(texture)
	{
		texture_new = texture->Duplicate();
		if(!texture_new)
			return FALSE;
		to->AddTexture(texture_new);
		texture = (TEXTURE*)texture->GetNext();
	}
	// Copy brushes
	brush = from->brush;
	while(brush)
	{
		brush_new = new BRUSH;
		if(!brush_new)
			return FALSE;
#ifdef __STORM__
		// Bug in StormC introduced with 2.0.23
		memcpy(brush_new, brush, sizeof(BRUSH));
#else
		*brush_new = *brush;
#endif // __STORMC__
		to->AddBrush(brush_new);
		brush = (BRUSH*)brush->GetNext();
	}
	return TRUE;
}

/*************
 * DESCRIPTION:   called to add a sphere
 * INPUT:         -
 * OUTPUT:        created sphere
 *************/
static OBJECT* SAVEDS SphereAdd(RSICONTEXT *rc)
{
	SPHERE *sphere;

	sphere = new SPHERE;
	if(!sphere)
		return NULL;

	if(rc->cur_csg)
		 *rc->cur_node = sphere;
	else
		sphere->Insert(rc);

	return sphere;
}

/*************
 * DESCRIPTION:   called to set sphere properties
 * INPUT:         sphere
 *                pos      position
 *                r        radius
 * OUTPUT:        created sphere
 *************/
static void SAVEDS SphereAxis(RSICONTEXT *rc, OBJECT *sphere, VECTOR *pos, float r)
{
	((SPHERE*)sphere)->pos = *pos;
	((SPHERE*)sphere)->r = r;
}

/*************
 * DESCRIPTION:   called to add triangles
 * INPUT:         count    amount of triangles
 *                surf     surface
 *                actor    actor
 * OUTPUT:        created triangles or NULL if failed
 *************/
static TRIANGLE* SAVEDS TriangleAdd(RSICONTEXT *rc, int count,SURFACE *surf,ACTOR *actor)
{
	TRIANGLE *tria;
#ifdef __STORM__
	TRIANGLE *first = NULL;
#endif // __STORM__
	int i;

	if(!count)
		return NULL;

#ifndef __STORM__
	// there is a bug in StormC which causes a crash if an array
	// is allocated when the destructor of the base class is virtual.
	// I have to test again if it works when a new version is available.
	// Do also change TriangleGetNext().
	tria = new TRIANGLE[count];
	if(!tria)
		return NULL;
	tria[0].flags |= OBJECT_FIRSTBLOCK;

	for(i=0; i<count; i++)
	{
		tria[i].surf = surf;
		tria[i].actor = actor;
		if(i!=0)
			tria[i].flags |= OBJECT_INBLOCK;
		tria[i].Insert(rc);
	}
#else // __STORM__
	for(i=0; i<count; i++)
	{
		tria = new TRIANGLE;
		if(!tria)
			return first;
		if(!first)
			first = tria;
		tria->surf = surf;
		tria->actor = actor;
		tria->Insert(rc);
	}
#endif // __STORM__

	return tria;
}

/*************
 * DESCRIPTION:   called to set points of triangle
 * INPUT:         tria     current triangle
 *                p1,p2,p3 points
 * OUTPUT:        -
 *************/
static void SAVEDS TrianglePoints(RSICONTEXT *rc, TRIANGLE *tria,VECTOR *p1,VECTOR *p2,VECTOR *p3)
{
	tria->p[0] = *p1;
	tria->p[1] = *p2;
	tria->p[2] = *p3;
}

/*************
 * DESCRIPTION:   called to set edge normals of triangle
 * INPUT:         tria     triangle
 *                v1,v2,v3 edge normals
 * OUTPUT:        FALSE if failed else TRUE
 *************/
static BOOL SAVEDS TriangleVNorm(RSICONTEXT *rc, TRIANGLE *tria,VECTOR *v1,VECTOR *v2,VECTOR *v3)
{
	tria->vnorm = new VECTOR[3];
	if(!tria->vnorm)
		return FALSE;
	tria->vnorm[0] = *v1;
	tria->vnorm[1] = *v2;
	tria->vnorm[2] = *v3;
	return TRUE;
}

/*************
 * DESCRIPTION:   called to set brush mapping coordinates of triangle
 * INPUT:         tria     triangle
 *                m1,m2,m3 mapping coordinates
 * OUTPUT:        FALSE if failed else TRUE
 *************/
static BOOL SAVEDS TriangleMapping(RSICONTEXT *rc, TRIANGLE *tria,VECT2D *m1,VECT2D *m2,VECT2D *m3)
{
	tria->brushcoord = new VECT2D[3];
	if(!tria->brushcoord)
		return FALSE;
	tria->brushcoord[0] = *m1;
	tria->brushcoord[1] = *m2;
	tria->brushcoord[2] = *m3;
	return TRUE;
}

/*************
 * DESCRIPTION:   called to set the surface of a triangle
 * INPUT:         surf
 * OUTPUT:        none
 *************/
static void SAVEDS TriangleSurface(RSICONTEXT *rc, TRIANGLE *tria, SURFACE *surf)
{
	tria->surf = surf;
}

/*************
 * DESCRIPTION:   called to get next triangle
 * INPUT:         tria     triangle
 * OUTPUT:        next triangle
 *************/
static TRIANGLE* SAVEDS TriangleGetNext(RSICONTEXT *rc, TRIANGLE *tria)
{
#ifdef __STORM__
	return (TRIANGLE*)tria->GetNext();
#else
	return &tria[1];
#endif
}

/*************
 * DESCRIPTION:   called to add a mesh
 * INPUT:         srf         surface
 *                surfs       surface list
 *                actor       actor
 *                points      mesh points
 *                pointcount  amount of points
 *                edges       mesh edges
 *                edgecount   amount of edges
 *                mesh        mesh triangles
 *                triacount   amount of triangles
 *                pos         position
 *                ox,oy,oz    orientation
 *                size        size
 *                nophong     disable phong shading
 * OUTPUT:        first created triangle
 *************/
static OBJECT* SAVEDS MeshAddRenderer(RSICONTEXT *rc, SURFACE *srf, SURFACE **surfs, ACTOR *actor, VECTOR *points, int pointcount,
	LINK_EDGE *edges, int edgecount, LINK_MESH *mesh, int triacount,
	VECTOR *pos, VECTOR *ox, VECTOR *oy, VECTOR *oz, VECTOR *size, BOOL nophong)
{
	MATRIX m, rot;

	m.SetSOTMatrix(size, ox, oy, oz, pos);
	rot.SetOMatrix(ox, oy, oz);

	return (OBJECT*)CreateMesh(rc, srf, surfs, actor, points, (EDGE*)edges, edgecount, (MESH*)mesh, triacount, &m, &rot, nophong, NULL);
}

/*************
 * DESCRIPTION:   called to add a box
 * INPUT:         -
 * OUTPUT:        created box
 *************/
static OBJECT* SAVEDS BoxAdd(RSICONTEXT *rc)
{
	BOX *box;

	box = new BOX;
	if(!box)
		return NULL;
	if(rc->cur_csg)
		*rc->cur_node = box;
	else
		box->Insert(rc);
	return (OBJECT*)box;
}

/*************
 * DESCRIPTION:   called to set the bounds of a box
 * INPUT:         box
 *                lowbounds
 *                highbounds
 * OUTPUT:        -
 *************/
static void SAVEDS BoxBounds(RSICONTEXT *rc, OBJECT *box, VECTOR *lowbounds, VECTOR *highbounds)
{
	((BOX*)box)->lowbounds = *lowbounds;
	((BOX*)box)->highbounds = *highbounds;
}

/*************
 * DESCRIPTION:   called to set the position and orientation of a box
 * INPUT:         box
 *                pos         position
 *                ox,oy,oz    orientation
 * OUTPUT:        -
 *************/
static void SAVEDS BoxAxis(RSICONTEXT *rc, OBJECT *box, VECTOR *pos, VECTOR *ox, VECTOR *oy, VECTOR *oz)
{
	((BOX*)box)->pos = *pos;
	((BOX*)box)->orient_x = *ox;
	((BOX*)box)->orient_y = *oy;
	((BOX*)box)->orient_z = *oz;
}

/*************
 * DESCRIPTION:   called to add a cylinder
 * INPUT:         -
 * OUTPUT:        created cylinder
 *************/
static OBJECT* SAVEDS CylinderAdd(RSICONTEXT *rc)
{
	CYLINDER *cylinder;

	cylinder = new CYLINDER;
	if(!cylinder)
		return NULL;
	if(rc->cur_csg)
		*rc->cur_node = cylinder;
	else
		cylinder->Insert(rc);
	return (OBJECT*)cylinder;
}

/*************
 * DESCRIPTION:   called to set cylinder properties
 * INPUT:         cylinder
 *                pos         position
 *                ox,oy,oz    orientation
 *                size        size
 * OUTPUT:        -
 *************/
static void SAVEDS CylinderAxis(RSICONTEXT *rc, OBJECT *cylinder, VECTOR *pos, VECTOR *ox, VECTOR *oy, VECTOR *oz, VECTOR *size)
{
	((CYLINDER*)cylinder)->pos = *pos;
	((CYLINDER*)cylinder)->ox = *ox;
	((CYLINDER*)cylinder)->oy = *oy;
	((CYLINDER*)cylinder)->oz = *oz;
	((CYLINDER*)cylinder)->size = *size;
}

/*************
 * FUNCTION:      CylinderFlags
 * DESCRIPTION:   called to set cylinder flags
 * INPUT:         cylinder cylinder
 *                flags    flags
 * OUTPUT:        -
 *************/
static void SAVEDS CylinderFlags(RSICONTEXT *rc, OBJECT *cylinder, ULONG flags)
{
	if(!(flags & LINK_CYLINDER_OPENTOP))
		cylinder->flags |= OBJECT_CLOSETOP;
	if(!(flags & LINK_CYLINDER_OPENBOTTOM))
		cylinder->flags |= OBJECT_CLOSEBOTTOM;
}

/*************
 * DESCRIPTION:   called to add a cone
 * INPUT:         -
 * OUTPUT:        created cone
 *************/
static OBJECT* SAVEDS ConeAdd(RSICONTEXT *rc)
{
	CONE *cone;

	cone = new CONE;
	if(!cone)
		return NULL;
	if(rc->cur_csg)
		*rc->cur_node = cone;
	else
		cone->Insert(rc);

	return (OBJECT*)cone;
}

/*************
 * DESCRIPTION:   called to set cone properties
 * INPUT:         cone
 *                pos         position
 *                ox,oy,oz    orientation
 *                size        size
 * OUTPUT:        -
 *************/
static void SAVEDS ConeAxis(RSICONTEXT *rc, OBJECT *cone, VECTOR *pos, VECTOR *ox, VECTOR *oy, VECTOR *oz, VECTOR *size)
{
	((CONE*)cone)->pos = *pos;
	((CONE*)cone)->ox = *ox;
	((CONE*)cone)->oy = *oy;
	((CONE*)cone)->oz = *oz;
	((CONE*)cone)->size = *size;
}

/*************
 * DESCRIPTION:   called to set cone flags
 * INPUT:         cone cone
 *                flags    flags
 * OUTPUT:        -
 *************/
static void SAVEDS ConeFlags(RSICONTEXT *rc, OBJECT *cone, ULONG flags)
{
	if(!(flags & LINK_CONE_OPENBOTTOM))
		cone->flags |= OBJECT_CLOSEBOTTOM;
}

/*************
 * DESCRIPTION:   called to add a plane
 * INPUT:         -
 * OUTPUT:        created plane
 *************/
static OBJECT* SAVEDS PlaneAdd(RSICONTEXT *rc)
{
	PLANE *plane;

	plane = new PLANE;
	if(!plane)
		return NULL;
	plane->Insert(rc);
	return plane;
}

/*************
 * DESCRIPTION:   called to set the plane properties
 * INPUT:         plane
 *                pos      position
 *                norm     normal
 * OUTPUT:        -
 *************/
static void SAVEDS PlaneAxis(RSICONTEXT *rc, OBJECT *plane, VECTOR *pos, VECTOR *normal)
{
	((PLANE*)plane)->pos = *pos;
	((PLANE*)plane)->norm = *normal;
}

/*************
 * DESCRIPTION:   called to add a CSG object
 * INPUT:         -
 * OUTPUT:        created CSG object
 *************/
static OBJECT* SAVEDS CSGAdd(RSICONTEXT *rc)
{
	CSG *csg;
	HIERARCHIE *hier;

	csg = new CSG;
	if(!csg)
		return NULL;

	hier = new HIERARCHIE;
	if(!hier)
	{
		delete csg;
		return NULL;
	}
	hier->node = csg;
	hier->prev = rc->cur_hier;
	rc->cur_hier = hier;

	if(rc->cur_csg)
		*rc->cur_node = csg;
	else
		csg->Insert(rc);

	rc->cur_csg = csg;
	rc->cur_node = &rc->cur_csg->obj1;

	return csg;
}

/*************
 * DESCRIPTION:   called to set CSG operation
 * INPUT:         csg      csg object
 *                operation
 * OUTPUT:        -
 *************/
static void SAVEDS CSGOperation(RSICONTEXT *rc, OBJECT *csg, UWORD operation)
{
	csg->flags &= ~OBJECT_CSG_MASK;
	switch(operation)
	{
		case LINK_CSG_UNION:
			csg->flags |= OBJECT_CSG_UNION;
			break;
		case LINK_CSG_INTERSECTION:
			csg->flags |= OBJECT_CSG_INTERSECT;
			break;
		case LINK_CSG_DIFFERENCE:
			csg->flags |= OBJECT_CSG_DIFFERENCE;
			break;
	}
}

/*************
 * DESCRIPTION:   called to add a SOR
 * INPUT:         -
 * OUTPUT:        created SOR
 *************/
static OBJECT* SAVEDS SORAdd(RSICONTEXT *rc)
{
	SOR *sor;

	sor = new SOR;
	if(!sor)
		return NULL;
	if(rc->cur_csg)
		*rc->cur_node = sor;
	else
		sor->Insert(rc);
	return (OBJECT*)sor;
}

/*************
 * DESCRIPTION:   called to set SOR properties
 * INPUT:         sor
 *                pos         position
 *                ox,oy,oz    orientation
 *                size        size
 * OUTPUT:        -
 *************/
static void SAVEDS SORAxis(RSICONTEXT *rc, OBJECT *sor, VECTOR *pos, VECTOR *ox, VECTOR *oy, VECTOR *oz, VECTOR *size)
{
	((SOR*)sor)->pos = *pos;
	((SOR*)sor)->orient_x = *ox;
	((SOR*)sor)->orient_y = *oy;
	((SOR*)sor)->orient_z = *oz;
	((SOR*)sor)->size = *size;
}

/*************
 * DESCRIPTION:   called to set SOR flags
 * INPUT:         sor
 *                flags
 * OUTPUT:        -
 *************/
static void SAVEDS SORFlags(RSICONTEXT *rc, OBJECT *sor, ULONG flags)
{
	if(!(flags & LINK_SOR_OPENTOP))
		sor->flags |= OBJECT_CLOSETOP;
	if(!(flags & LINK_SOR_OPENBOTTOM))
		sor->flags |= OBJECT_CLOSEBOTTOM;
	if(flags & LINK_SOR_ACCURATE)
		sor->flags |= OBJECT_SORSTURM;
}

/*************
 * DESCRIPTION:   called to add points to a SOR object
 * INPUT:         sor
 *                points
 *                count
 * OUTPUT:        FALSE if failed, else TRUE
 *************/
static BOOL SAVEDS SORPoints(RSICONTEXT *rc, OBJECT *sor, VECT2D *points, ULONG count)
{
	((SOR*)sor)->number = count-3;
	if(!((SOR*)sor)->Compute(points))
		return FALSE;

	return TRUE;
}

/*************
 * DESCRIPTION:   called to add a texture
 * INPUT:         name     texture name
 *                surf     surface
 * OUTPUT:        created texture
 *************/
static TEXTURE* SAVEDS ITextureAdd(RSICONTEXT *rc, char *name,SURFACE *surf)
{
	IMAGINE_TEXTURE *texture;

	texture = new IMAGINE_TEXTURE;
	if(!texture)
		return NULL;
	if(texture->Load(rc, name, rc->world.texturepath) != ERR_NONE)
	{
		delete texture;
		return NULL;
	}
	surf->AddTexture(texture);
	return (TEXTURE*)texture;
}

/*************
 * DESCRIPTION:   called to set texture parameters
 * INPUT:         texture     texture
 *                i           number
 *                param       value
 * OUTPUT:        -
 *************/
static void SAVEDS ITextureParams(RSICONTEXT *rc, TEXTURE *texture,int i,float param)
{
	((IMAGINE_TEXTURE*)texture)->param[i] = param;
}

/*************
 * DESCRIPTION:   called to add a texture
 * INPUT:         name     texture name
 *                surf     surface
 * OUTPUT:        created texture
 *************/
static TEXTURE* SAVEDS RSTextureAdd(RSICONTEXT *rc, char *name,SURFACE *surf)
{
	RAYSTORM_TEXTURE *texture;

	texture = new RAYSTORM_TEXTURE;
	if(!texture)
		return NULL;
	if(texture->Load(rc, name, rc->world.texturepath) != ERR_NONE)
	{
		delete texture;
		return NULL;
	}
	surf->AddTexture(texture);
	return (TEXTURE*)texture;
}

/*************
 * DESCRIPTION:   called to set texture parameters
 * INPUT:         texture     texture
 *                size        size of parameter structure
 *                params      paramters
 * OUTPUT:        -
 *************/
static BOOL SAVEDS RSTextureParams(RSICONTEXT *rc, TEXTURE *texture,int size, UBYTE *params)
{
	((RAYSTORM_TEXTURE*)texture)->data = new UBYTE[size];
	if(!((RAYSTORM_TEXTURE*)texture)->data)
		return FALSE;
	memcpy(((RAYSTORM_TEXTURE*)texture)->data, params, size);
	return TRUE;
}

/*************
 * DESCRIPTION:   called to add a texture axis definition to an object
 * INPUT:         obj      object
 *                actor
 *                prev     previous created texture object (NULL for first)
 * OUTPUT:        created texture object
 *************/
static TEXTURE_OBJECT* SAVEDS TextureObjectAdd(RSICONTEXT *rc, OBJECT *obj, ACTOR *actor, TEXTURE_OBJECT *prev)
{
	TEXTURE *texture;

	if(prev)
		texture = (TEXTURE*)((TEXTURE*)prev)->GetNext();
	else
		texture = obj->surf->texture;

	texture->actor = actor;

	return (TEXTURE_OBJECT*)texture;
}

/*************
 * DESCRIPTION:   called to set texture position
 * INPUT:         texture     texture
 *                pos         position
 * OUTPUT:        -
 *************/
static void SAVEDS TexturePos(RSICONTEXT *rc, TEXTURE_OBJECT *texture,VECTOR *pos)
{
	((TEXTURE*)texture)->pos = *pos;
}

/*************
 * DESCRIPTION:   called to set texture orientation
 * INPUT:         texture     texture
 *                orient_x,orient_y,orient_z    orientation
 * OUTPUT:        -
 *************/
static void SAVEDS TextureOrient(RSICONTEXT *rc, TEXTURE_OBJECT *texture,VECTOR *orient_x,VECTOR *orient_y,VECTOR *orient_z)
{
	((TEXTURE*)texture)->orient_x = *orient_x;
	((TEXTURE*)texture)->orient_y = *orient_y;
	((TEXTURE*)texture)->orient_z = *orient_z;
}

/*************
 * DESCRIPTION:   called to set texture size
 * INPUT:         texture     texture
 *                size        size
 * OUTPUT:        -
 *************/
static void SAVEDS TextureSize(RSICONTEXT *rc, TEXTURE_OBJECT *texture,VECTOR *size)
{
	((TEXTURE*)texture)->size = *size;
}

/*************
 * DESCRIPTION:   called to add a new brush
 * INPUT:         name     brush name
 *                surf     surface
 * OUTPUT:        created brush
 *************/
static BRUSH* SAVEDS BrushAdd(RSICONTEXT *rc, char *name,SURFACE *surf)
{
	BRUSH *brush;

	brush = new BRUSH;
	if(!brush)
		return NULL;

	// look if image already loaded
	brush->image = FindImage(rc, name);
	if(!brush->image)
	{
		// didn't found the image => load it
		brush->image = new IMAGE;
		if(!brush->image)
		{
			delete brush;
			return NULL;
		}
		if(brush->image->Load(rc, name, rc->world.brushpath))
		{
			delete brush->image;
			delete brush;
			return NULL;
		}
		brush->image->Insert(rc);
	}
	surf->AddBrush(brush);

	return brush;
}

/*************
 * DESCRIPTION:   called to add a brush axis definition to an object
 * INPUT:         obj      object
 *                actor    actor
 *                prev     previous created brush object (NULL for first)
 * OUTPUT:        created brushobject
 *************/
static BRUSH_OBJECT* SAVEDS BrushObjectAdd(RSICONTEXT *rc, OBJECT *obj, ACTOR *actor, BRUSH_OBJECT *prev)
{
	BRUSH *brush;

	if(prev)
		brush = (BRUSH*)((BRUSH*)prev)->GetNext();
	else
		brush = obj->surf->brush;

	brush->actor = actor;

	return (BRUSH_OBJECT*)brush;
}

/*************
 * DESCRIPTION:   called to set brush flags
 * INPUT:         brush    brush
 *                flags    flags
 * OUTPUT:        -
 *************/
static void SAVEDS BrushFlags(RSICONTEXT *rc, BRUSH *brush,ULONG flags)
{
	switch(flags & LINK_BRUSH_MAP_MASK)
	{
		default:
		case LINK_BRUSH_MAP_COLOR:
			brush->flags = MAP_COLOR;
			break;
		case LINK_BRUSH_MAP_REFLECTIFITY:
			brush->flags = MAP_REFLECTIFITY;
			break;
		case LINK_BRUSH_MAP_FILTER:
			brush->flags = MAP_FILTER;
			break;
		case LINK_BRUSH_MAP_ALTITUDE:
			brush->flags = MAP_ALTITUDE;
			break;
		case LINK_BRUSH_MAP_SPECULAR:
			brush->flags = MAP_SPECULAR;
			break;
		case LINK_BRUSH_MAP_REFLECTION:
			brush->flags = MAP_REFLECTION;
			break;
	}

	if(flags & LINK_BRUSH_SOFT)
		brush->flags |= SOFT;
	if(flags & LINK_BRUSH_WRAPX)
		brush->flags |= WRAP_X;
	if(flags & LINK_BRUSH_WRAPY)
		brush->flags |= WRAP_Y;

	if(flags & LINK_BRUSH_REPEAT)
		brush->flags |= REPEAT;
	if(flags & LINK_BRUSH_MIRROR)
		brush->flags |= MIRROR;
}

/*************
 * DESCRIPTION:   called to set brush position
 * INPUT:         brush    brush
 *                pos      position
 * OUTPUT:        -
 *************/
static void SAVEDS BrushPos(RSICONTEXT *rc, BRUSH_OBJECT *brush,VECTOR *pos)
{
	((BRUSH*)brush)->pos = *pos;
}

/*************
 * DESCRIPTION:   called to set brush orientation
 * INPUT:         orient_x,orient_y,orient_z    orientation
 * OUTPUT:        -
 *************/
static void SAVEDS BrushOrient(RSICONTEXT *rc, BRUSH_OBJECT *brush,VECTOR *orient_x,VECTOR *orient_y,VECTOR *orient_z)
{
	((BRUSH*)brush)->orient_x = *orient_x;
	((BRUSH*)brush)->orient_y = *orient_y;
	((BRUSH*)brush)->orient_z = *orient_z;
}

/*************
 * DESCRIPTION:   called to set brush size
 * INPUT:         brush    brush
 *                size     size
 * OUTPUT:        -
 *************/
static void SAVEDS BrushSize(RSICONTEXT *rc, BRUSH_OBJECT *brush,VECTOR *size)
{
	((BRUSH*)brush)->size = *size;
}

/*************
 * DESCRIPTION:   called to set brush alignment
 * INPUT:         brush    brush
 *                algin    alignment
 * OUTPUT:        -
 *************/
static void SAVEDS BrushAlignment(RSICONTEXT *rc, BRUSH_OBJECT *brush,VECTOR *align)
{
	((BRUSH*)brush)->align = *align;
}

/*************
 * DESCRIPTION:   called to set the light position
 * INPUT:         light    light
 *                pos      position
 * OUTPUT:        -
 *************/
static void SAVEDS LightPos(RSICONTEXT *rc, LIGHT *light, VECTOR *pos)
{
	light->pos = *pos;
}

/*************
 * DESCRIPTION:   called to set the light color
 * INPUT:         light    light
 *                r,g,b    color
 * OUTPUT:        -
 *************/
static void SAVEDS LightColor(RSICONTEXT *rc, LIGHT *light, float r, float g, float b)
{
	light->color.r = r;
	light->color.g = g;
	light->color.b = b;
}

/*************
 * DESCRIPTION:   called to set the flags of the light
 * INPUT:         light    light
 *                flags    flags
 * OUTPUT:        -
 *************/
static void SAVEDS LightFlags(RSICONTEXT *rc, LIGHT *light, ULONG flags)
{
	if(flags & LINK_LIGHT_SHADOW)
		light->flags |= LIGHT_SHADOW;
	if(flags & LINK_LIGHT_FLARES)
		light->flags |= LIGHT_FLARES;
	if(flags & LINK_LIGHT_STAR)
		light->flags |= LIGHT_STAR;
	if(flags & LINK_LIGHT_SHADOWMAP)
		light->flags |= LIGHT_SHADOWMAP;
}

/*************
 * DESCRIPTION:   called to set the light falloff value
 * INPUT:         light    light
 *                falloff  falloff value
 * OUTPUT:        -
 *************/
static void SAVEDS LightFallOff(RSICONTEXT *rc, LIGHT *light, float falloff)
{
	light->falloff = falloff;
}

/*************
 * DESCRIPTION:   called to set the light radius
 * INPUT:         light    light
 *                r        radius
 * OUTPUT:        -
 *************/
static void SAVEDS LightRadius(RSICONTEXT *rc, LIGHT *light, float r)
{
	light->r = r;
}

/*************
 * DESCRIPTION:   called to set the size of the shadowmap
 * INPUT:         light    light
 *                size     size of shadowmap
 * OUTPUT:        -
 *************/
static void SAVEDS LightShadowMapSize(RSICONTEXT *rc, LIGHT *light, int size)
{
	light->shadowmap_size = size;
}

/*************
 * DESCRIPTION:   called to add a point light
 * INPUT:         -
 * OUTPUT:        created pointlight or NULL if failed
 *************/
static LIGHT* SAVEDS PointLightAdd(RSICONTEXT *rc)
{
	POINT_LIGHT *light;

	light = new POINT_LIGHT;
	if(!light)
		return NULL;
	light->Insert(rc);
	return light;
}

/*************
 * DESCRIPTION:   called to add a spot light
 * INPUT:         -
 * OUTPUT:        created spotlight or NULL if failed
 *************/
static LIGHT* SAVEDS SpotLightAdd(RSICONTEXT *rc)
{
	SPOT_LIGHT *light;

	light = new SPOT_LIGHT;
	if(!light)
		return NULL;
	light->Insert(rc);
	return light;
}

/*************
 * DESCRIPTION:   called to set the spot light opening angle
 * INPUT:         light    light
 *                angle    opening angle
 * OUTPUT:        -
 *************/
static void SAVEDS SpotLightAngle(RSICONTEXT *rc, LIGHT *light, float angle)
{
	((SPOT_LIGHT*)light)->angle = angle;
}

/*************
 * DESCRIPTION:   called to set the spot light falloff radius
 * INPUT:         light    light
 *                falloff  falloff radius
 * OUTPUT:        -
 *************/
static void SAVEDS SpotLightFallOffRadius(RSICONTEXT *rc, LIGHT *light, float falloff)
{
	((SPOT_LIGHT*)light)->falloff_radius = falloff;
}

/*************
 * DESCRIPTION:   called to add a directional light
 * INPUT:         -
 * OUTPUT:        created pointlight or NULL if failed
 *************/
static LIGHT* SAVEDS DirectionalLightAdd(RSICONTEXT *rc)
{
	DIRECTIONAL_LIGHT *light;

	light = new DIRECTIONAL_LIGHT;
	if(!light)
		return NULL;
	light->Insert(rc);
	return light;
}

/*************
 * DESCRIPTION:   called to add a flare to a light source
 * INPUT:         light    light to add flare to
 * OUTPUT:        created flare or NULL if failed
 *************/
static FLARE* SAVEDS FlareAdd(RSICONTEXT *rc, LIGHT *light)
{
	FLARE *flare;

	flare = new FLARE;
	if(!flare)
		return NULL;

	flare->Insert((SLIST**)&light->flares);

	return flare;
}

/*************
 * DESCRIPTION:   called to set the flare color
 * INPUT:         flare    pointer to current flare
 *                r,g,b    color
 * OUTPUT:        -
 *************/
static void SAVEDS FlareColor(RSICONTEXT *rc, FLARE *flare, float r, float g, float b)
{
	flare->color.r = r;
	flare->color.g = g;
	flare->color.b = b;
}

/*************
 * DESCRIPTION:   called to set the flare position
 * INPUT:         flare    pointer to current flare
 *                pos      position
 * OUTPUT:        -
 *************/
static void SAVEDS FlarePosition(RSICONTEXT *rc, FLARE *flare, float pos)
{
	flare->pos = pos;
}

/*************
 * DESCRIPTION:   called to set the flare radius
 * INPUT:         flare    pointer to current flare
 *                r        radius
 * OUTPUT:        -
 *************/
static void SAVEDS FlareRadius(RSICONTEXT *rc, FLARE *flare, float r)
{
	flare->radius = r;
}

/*************
 * DESCRIPTION:   called to set the flare type
 * INPUT:         flare    pointer to current flare
 *                type     flare type
 * OUTPUT:        -
 *************/
static void SAVEDS FlareType(RSICONTEXT *rc, FLARE *flare, ULONG type)
{
	flare->type = type;
}

/*************
 * DESCRIPTION:   called to set the flare interpolation function
 * INPUT:         flare    pointer to current flare
 *                func     function
 * OUTPUT:        -
 *************/
static void SAVEDS FlareFunction(RSICONTEXT *rc, FLARE *flare, ULONG func)
{
	flare->func = func;
}

/*************
 * DESCRIPTION:   called to set the number of flare edges
 * INPUT:         flare    pointer to current flare
 *                num      number of edges
 * OUTPUT:        -
 *************/
static void SAVEDS FlareEdges(RSICONTEXT *rc, FLARE *flare, ULONG num)
{
	flare->edges = num;
}

/*************
 * DESCRIPTION:   called to set the flare tilt angle
 * INPUT:         flare    pointer to current flare
 *                tilt     tilt angle
 * OUTPUT:        -
 *************/
static void SAVEDS FlareTilt(RSICONTEXT *rc, FLARE *flare, float tilt)
{
	flare->tilt = tilt;
}

/*************
 * DESCRIPTION:   called to add a star to a light source
 * INPUT:         light    light to add star to
 * OUTPUT:        created star or NULL if failed
 *************/
static STAR* SAVEDS StarAdd(RSICONTEXT *rc, LIGHT *light)
{
	if(light->star)
		delete light->star;

	light->star = new STAR;
	if(!light->star)
		return NULL;

	return light->star;
}

/*************
 * DESCRIPTION:   called to set the star flags
 * INPUT:         star     pointer to current star
 *                flags    flags
 * OUTPUT:        -
 *************/
static void SAVEDS StarFlags(RSICONTEXT *rc, STAR *star, ULONG flags)
{
	if(flags & LINK_STAR_ENABLE)
		star->flags |= STAR_ENABLE;
	if(flags & LINK_STAR_HALOENABLE)
		star->flags |= STAR_HALOENABLE;
}

/*************
 * DESCRIPTION:   called to set the star radius
 * INPUT:         star     pointer to current star
 *                r        radius
 * OUTPUT:        -
 *************/
static void SAVEDS StarRadius(RSICONTEXT *rc, STAR *star, float radius)
{
	star->starradius = radius;
}

/*************
 * DESCRIPTION:   called to set the star tilt angle
 * INPUT:         star     pointer to current star
 *                tilt     tilt angle
 * OUTPUT:        -
 *************/
static void SAVEDS StarTilt(RSICONTEXT *rc, STAR *star, float tilt)
{
	star->tilt = tilt;
}

/*************
 * DESCRIPTION:   called to set the amount of star spikes
 * INPUT:         star     pointer to current star
 *                spikes   amount of spikes
 * OUTPUT:        -
 *************/
static void SAVEDS StarSpikes(RSICONTEXT *rc, STAR *star, ULONG spikes)
{
	star->spikes = spikes;
}

/*************
 * DESCRIPTION:   called to set the halo radius of the star
 * INPUT:         star     pointer to current star
 *                r        halo radius
 * OUTPUT:        -
 *************/
static void SAVEDS StarHaloradius(RSICONTEXT *rc, STAR *star, float r)
{
	star->haloradius = r;
}

/*************
 * DESCRIPTION:   called to set the inner halo radius of the star
 * INPUT:         star     pointer to current star
 *                r        inner halo radius
 * OUTPUT:        -
 *************/
static void SAVEDS StarInnerHaloradius(RSICONTEXT *rc, STAR *star, float r)
{
	star->innerhaloradius = r;
}

/*************
 * DESCRIPTION:   called to set star spikes range
 * INPUT:         star     pointer to current star
 *                range    spikes range
 * OUTPUT:        -
 *************/
static void SAVEDS StarRange(RSICONTEXT *rc, STAR *star, float range)
{
}

/*************
 * DESCRIPTION:   called to set the star intensities
 * INPUT:         star     pointer to current star
 *                num      amount of intensities
 *                inte     array of intensities
 * OUTPUT:        TRUE if all ok else FALSE
 *************/
static BOOL SAVEDS StarIntensities(RSICONTEXT *rc, STAR *star, int num, float *inte)
{
	if(star->intensities)
		delete star->intensities;

	star->intensities = new float[num];
	if(!star->intensities)
		return FALSE;

	memcpy(star->intensities, inte, sizeof(float)*num);

	return TRUE;
}

static OBJLINK link =
{
	LINK_RENDERER,

	ObjectBegin,
	ObjectEnd,
	ObjectSurface,
	NULL,             // ObjectName
	ObjectAxis,
	ObjectActor,
	ObjectFlags,
	NULL,             // ObjectTrack

	SurfaceAdd,
	SurfaceGetByName,
	SurfaceName,
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
	SurfaceCopy,

	SphereAdd,
	SphereAxis,

	TriangleAdd,
	NULL,             // TriangleAddPoints
	NULL,             // TriangleAddEdges
	NULL,             // TriangleSetEdges
	NULL,             // TriangleSetEdgeFlags)
	TrianglePoints,
	TriangleVNorm,
	TriangleMapping,
	TriangleSurface,
	TriangleGetNext,

	NULL,             // MeshCreate
	NULL,             // MeshAddScenario
	MeshAddRenderer,

	BoxAdd,
	BoxBounds,
	BoxAxis,

	CylinderAdd,
	CylinderAxis,
	CylinderFlags,

	ConeAdd,
	ConeAxis,
	ConeFlags,

	PlaneAdd,
	PlaneAxis,

	CSGAdd,
	CSGOperation,

	SORAdd,
	SORAxis,
	SORFlags,
	SORPoints,

	ITextureAdd,
	ITextureParams,

	RSTextureAdd,
	RSTextureParams,

	TextureObjectAdd,
	TexturePos,
	TextureOrient,
	TextureSize,

	BrushAdd,
	BrushObjectAdd,
	BrushFlags,
	BrushPos,
	BrushOrient,
	BrushSize,
	BrushAlignment,

	NULL,             // CameraAdd
	NULL,             // CameraFlags
	NULL,             // CameraFocalDist
	NULL,             // CameraAperture
	NULL,             // CameraFOV

	LightPos,
	LightColor,
	LightFlags,
	LightFallOff,
	LightRadius,
	LightShadowMapSize,

	PointLightAdd,

	SpotLightAdd,
	SpotLightAngle,
	SpotLightFallOffRadius,

	DirectionalLightAdd,

	FlareAdd,
	FlareColor,
	FlarePosition,
	FlareRadius,
	FlareType,
	FlareFunction,
	FlareEdges,
	FlareTilt,

	StarAdd,
	StarFlags,
	StarRadius,
	StarTilt,
	StarSpikes,
	StarHaloradius,
	StarInnerHaloradius,
	StarRange,
	StarIntensities
};

/*************
 * FUNCTION:      ReadObject
 * DESCRIPTION:   read a 3d object; invokes the right handler after inspecting
 *    the file. Checks the first 16 Byte of the object file.
 * INPUT:         rc          render context
 *                filename    name of object file
 *                pos         object position
 *                ox,oy,oz    object orientation
 *                actor       pointer to actor
 *                surf        pointer to surface to replace object surface with
 * OUTPUT         ERR_NONE if ok, else error number
 *************/
int ReadObject(RSICONTEXT *rc, char* filename, VECTOR *pos, VECTOR *ox, VECTOR *oy, VECTOR *oz,
	VECTOR *scale, ACTOR *actor, SURFACE *surf)
{
	char buffer[256];
	FILE *typefile;
	FILE *objfile;
	char typebuf[18];
	char objbuf[16];
	char *errstr;
#ifdef __AMIGA__
	char handlername[32];
#else
	HINSTANCE hLib;
	INITPROC objInit;
	READWRITEPROC objRead;
	CLEANUPPROC objCleanup;
	char buf[256];
#endif
	int i;
	BOOL identok=FALSE;
	UWORD mask;
	ULONG version;

	if(!ExpandPath(rc->world.objectpath, filename, buffer))
	{
		/* cannot open object */
		return ERR_OPENOBJ;
	}

	objfile = fopen(buffer,"rb");
	if(!objfile)
		return ERR_OPENOBJ;
	if(fread(objbuf,1,16,objfile) != 16)
	{
		fclose(objfile);
		return ERR_READOBJ;
	}
	fclose(objfile);
#ifdef __AMIGA__
	// Amiga-version: get directory RayStorm was started from and
	// search file from there
	// get a lock to the program directory
	BPTR lock = GetProgramDir();
	// make program directory to current directory
	lock = CurrentDir(lock);
	typefile = fopen("modules/object/types","rb");
	// change current dir to old dir
	lock = CurrentDir(lock);
#else
	strcpy(buf, WorkingDirectory);
	strcat(buf, "\\modules\\object");
	AddPart(buf, "types.dat", 256);
	typefile = fopen(buf, "rb");
#endif
	if(!typefile)
	{
		return ERR_OPENOBJTYPEFILE;
	}

	while(!feof(typefile) && !identok)
	{
		// Read identification string
		// Format:
		// UWORD mask;          mask for bytes to test '1'->test; lowest bit -> first byte
		// UBYTE id[16];        bytes to compare
		// char name[8];        name of module
		if(fread(&mask,2,1,typefile) != 1)
		{
			fclose(typefile);
			return ERR_READOBJTYPEFILE;
		}
#ifdef __SWAP__
		mask = SwapW(mask);
#endif
		if(fread(typebuf,1,16,typefile) != 16)
		{
			fclose(typefile);
			return ERR_READOBJTYPEFILE;
		}

		// Compare first 16 bytes of object with identstring
		identok = TRUE;
		for(i=0; i<16; i++)
		{
			if(mask & 0x8000)
			{
				if(typebuf[i] != objbuf[i])
				{
					identok = FALSE;
					break;
				}
			}
			mask = mask << 1;
		}
		// Read object name (max. 8 chars)
		if(fread(typebuf,1,8,typefile) != 8)
		{
			fclose(typefile);
			return ERR_READOBJTYPEFILE;
		}
	}
	if(identok)
	{
		fclose(typefile);
		typebuf[8] = 0;
#ifdef __AMIGA__
		// open object handler library
		// make program directory to current directory
		lock = CurrentDir(lock);
		strcpy(handlername,"modules/object");
		AddPart(handlername,typebuf,32);
#ifdef __PPC__
		strcat(handlername, "ppc");
#endif

		// lock library base (busy wait is ugly, I know)
		while(LockLibBase);
		LockLibBase = TRUE;

		ObjHandBase = OpenLibrary(handlername, 2L);
		if(!ObjHandBase)
		{
			LockLibBase = FALSE;
			return ERR_WRONGOBJHANDLER;
		}

		// change current dir to old dir
		CurrentDir(lock);
#else
		strcpy(buf, WorkingDirectory);
		strcat(buf, "\\modules\\object");
		AddPart(buf, typebuf, 256);
		strcat(buf, ".DLL");

		if (!(hLib = LoadLibrary(buf)))
			return ERR_OBJHANDLER;
		if (!(objInit = (INITPROC)GetProcAddress(hLib, "objInit_")))
			return ERR_OBJHANDLER;
		if (!(objRead = (READWRITEPROC)GetProcAddress(hLib, "objRead_")))
			return ERR_OBJHANDLER;
		if (!(objCleanup = (CLEANUPPROC)GetProcAddress(hLib, "objCleanup_")))
			return ERR_OBJHANDLER;
#endif
		if(!objInit())
		{
#ifdef __AMIGA__
			CloseLibrary(ObjHandBase);
			LockLibBase = FALSE;
#else
			FreeLibrary(hLib);
#endif
			return ERR_OBJHANDLER;
		}
		// delete last error
		rc->SetObjHandError(NULL);

		rc->surfname_root->FreeList();
		rc->surfname_root = NULL;
		// CSG init
		rc->cur_csg = NULL;
		rc->cur_node = NULL;
		rc->cur_hier->FreeList();
		rc->cur_hier = NULL;

		// invoke handler
		errstr = objRead(rc, buffer, &link, pos, ox,oy,oz, scale, actor, surf, &version, NULL);
		rc->surfname_root->FreeList();
		rc->surfname_root = NULL;
		// normally no hierarchie elements should need to be
		// freed, nevertheless do it, maybe the loader failed
		// and left some elements
		rc->cur_hier->FreeList();
		rc->cur_hier = NULL;
		if(errstr)
		{
			rc->SetObjHandError(errstr);
			objCleanup();
#ifdef __AMIGA__
			CloseLibrary(ObjHandBase);
			LockLibBase = FALSE;
#else
			FreeLibrary(hLib);
#endif
			return ERR_EXTERNAL;
		}
		// cleanup handler
		objCleanup();
		// close library
#ifdef __AMIGA__
		CloseLibrary(ObjHandBase);
		LockLibBase = FALSE;
#else
		FreeLibrary(hLib);
#endif
	}
	else
	{
		fclose(typefile);
		return ERR_UNKNOWNOBJ;
	}
	return ERR_NONE;
}
