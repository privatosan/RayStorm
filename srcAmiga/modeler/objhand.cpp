/***************
 * PROGRAM:       Modeler
 * NAME:          objhand.cpp
 * DESCRIPTION:   loader for external objects
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    20.03.95 ah    initial release
 *    29.11.96 ah    added Flare and Star functions
 *    25.01.97 ah    added AddBox()
 *    31.01.97 ah    added Star_Tilt()
 *    06.03.97 ah    added PointLight_ShadowMapSize() and SpotLight_ShadowMapSize()
 *    07.03.97 ah    added AddMesh()
 *    26.03.97 ah    added AddCylinder(), AddCone()
 *    16.05.97 ah    added SetName()
 *    09.06.97 ah    added handling to V2 style
 *    07.07.97 ah    added CSGAdd()
 *    26.09.97 ah    added SOR functions
 ***************/

#include <string.h>

#ifndef MESH_H
#include "mesh.h"
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

#ifndef ERRORS_H
#include "errors.h"
#endif

#ifndef BRUSH_H
#include "brush.h"
#endif

#ifndef TEXTURE_H
#include "texture.h"
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

#ifndef CAMERA_H
#include "camera.h"
#endif

#ifndef LIGHT_H
#include "light.h"
#endif

#ifndef FLARES_H
#include "flares.h"
#endif

#ifndef STAR_H
#include "star.h"
#endif

#ifndef OBJHAND_H
#include "objhand.h"
#endif

#ifndef SCI_H
#include "sci.h"
#endif

#ifndef __PPC__
// where to insert the next object
static OBJECT *where;

// insert direction
static int dir;

// undo object
static UNDO_CREATE *undo;

static INVMATRIX_STACK *stack;

#endif

#ifndef __PPC__
/*************
 * DESCRIPTION:   initialize object handler
 * INPUT:         root     root object
 *                direction   initial insert direction (usually INSERT_VERT)
 *                u        undo object (NULL if no undo)
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL InitObjHand(OBJECT *root, int direction, UNDO_CREATE *u)
{
	where = root;
	dir = direction;
	stack = new INVMATRIX_STACK;
	if(!stack)
		return FALSE;
	undo = u;

	return TRUE;
}

/*************
 * DESCRIPTION:   cleanup object handler
 * INPUT:         -
 * OUTPUT:        -
 *************/
void CleanupObjHand()
{
	if(stack)
		delete stack;
}
#endif

#if !defined(__MIXEDBINARY__) || defined(__PPC__)
/*************
 * DESCRIPTION:   called at start of new object
 * INPUT:         -
 * OUTPUT:        -
 *************/
void ObjectBegin(rsiCONTEXT *rc)
{
}
#endif

#ifndef __PPC__
/*************
 * DESCRIPTION:   called at end of an object
 * INPUT:         -
 * OUTPUT:        -
 *************/
void ObjectEnd(rsiCONTEXT *rc)
{
	stack->Pop();
	if(dir == INSERT_HORIZ)
	{
		while(where->GetPrev())
			where = (OBJECT*)where->GetPrev();
		if(where->GoUp())
			where = (OBJECT*)where->GoUp();
	}
	else
		dir = INSERT_HORIZ;

	where->CalcBBox();
}
#endif

#if !defined(__MIXEDBINARY__) || defined(__PPC__)
/*************
 * DESCRIPTION:   set the surface of the object
 * INPUT:         obj      object
 *                surface  surface
 * OUTPUT:        -
 *************/
void ObjectSurface(rsiCONTEXT *rc, OBJECT *obj, SURFACE *surface)
{
	obj->surf = surface;
}
#endif // __MIXEDBINARY__ || __PPC__

#ifndef __PPC__
/*************
 * DESCRIPTION:   called to set the name of an object
 * INPUT:         name
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL ObjectName(rsiCONTEXT *rc, OBJECT *obj, char *name)
{
	if(!obj->SetName(name))
		return FALSE;

	return TRUE;
}

/*************
 * DESCRIPTION:   called to set the object axis
 * INPUT:         pos      position
 *                orient_x,orient_y,orient_z    orientation
 *                size     size
 * OUTPUT:        -
 *************/
void ObjectAxis(rsiCONTEXT *rc, OBJECT *obj, VECTOR *pos, VECTOR *orient_x, VECTOR *orient_y, VECTOR *orient_z, VECTOR *size)
{
	MATRIX m, matrix;

	m.SetOTMatrix(orient_x, orient_y, orient_z, pos);
	matrix.MultMat(&m, &stack->matrix);
	SetVector(&obj->pos, matrix.m[1], matrix.m[2], matrix.m[3]);
	SetVector(&obj->orient_x, matrix.m[5], matrix.m[9], matrix.m[13]);
	SetVector(&obj->orient_y, matrix.m[6], matrix.m[10], matrix.m[14]);
	SetVector(&obj->orient_z, matrix.m[7], matrix.m[11], matrix.m[15]);
	matrix.InvMat(&m);
	stack->Push(&m);

	where->size = *size;
}

/*************
 * DESCRIPTION:   called to set the object the object tracks to
 * INPUT:         object      object
 *                track       name of tracked object
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL ObjectTrack(rsiCONTEXT *rc, OBJECT *object, char *track)
{
	object->track = (OBJECT*)new char[strlen(track)+1];
	if(!object->track)
		return FALSE;

	strcpy((char*)object->track, track);

	return TRUE;
}
#endif

#if !defined(__MIXEDBINARY__) || defined(__PPC__)
/*************
 * DESCRIPTION:   called to set object flags
 * INPUT:         object      object
 *                flags       flags
 * OUTPUT:        -
 *************/
void ObjectFlags(rsiCONTEXT *rc, OBJECT *object, ULONG flags)
{
	if(flags & LINK_OBJECT_INVERTED)
		object->flags |= OBJECT_INVERTED;
}
#endif

#ifndef __PPC__
/*************
 * DESCRIPTION:   called to add a new surface
 * INPUT:         -
 * OUTPUT:        created surface or NULL if failed
 *************/
SURFACE *SurfaceAdd(rsiCONTEXT *rc)
{
	SURFACE *surf;

	surf = sciCreateSurface(SURF_DEFAULTNAME);
	surf->flags |= SURF_EXTERNAL;
	return surf;
}

/*************
 * DESCRIPTION:   called to get a surface by its name
 * INPUT:         name
 * OUTPUT:        surface
 *************/
SURFACE *SurfaceGetByName(rsiCONTEXT *rc, char *name)
{
	return GetSurfaceByName(name);
}

/*************
 * DESCRIPTION:   called to set the surface name
 * INPUT:         surf     surface
 *                name     name
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL SurfaceName(rsiCONTEXT *rc, SURFACE *surf,char *name)
{
	return surf->SetName(name);
}
#endif

#if !defined(__MIXEDBINARY__) || defined(__PPC__)
/*************
 * DESCRIPTION:   called to set surface flags
 * INPUT:         surf     surface
 *                flags    flags
 * OUTPUT:        -
 *************/
void SurfaceSetFlags(rsiCONTEXT *rc, SURFACE *surf,UWORD flags)
{
	if(flags & LINK_SURFACE_BRIGHT)
		surf->flags |= SURF_BRIGHT;
}

/*************
 * DESCRIPTION:   called to set surface diffuse color
 * INPUT:         surf     surface
 *                r,g,b    color
 * OUTPUT:        -
 *************/
void SurfaceDiffuse(rsiCONTEXT *rc, SURFACE *surf,float r,float g,float b)
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
void SurfaceSpecular(rsiCONTEXT *rc, SURFACE *surf,float r,float g,float b)
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
void SurfaceAmbient(rsiCONTEXT *rc, SURFACE *surf,float r,float g,float b)
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
void SurfaceReflect(rsiCONTEXT *rc, SURFACE *surf,float r,float g,float b)
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
void SurfaceTranspar(rsiCONTEXT *rc, SURFACE *surf,float r,float g,float b)
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
void SurfaceDiffTrans(rsiCONTEXT *rc, SURFACE *surf, float r, float g, float b)
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
void SurfaceSpecTrans(rsiCONTEXT *rc, SURFACE *surf, float r, float g, float b)
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
void SurfaceFoglen(rsiCONTEXT *rc, SURFACE *surf,float flen)
{
	surf->foglength = flen;
}

/*************
 * DESCRIPTION:   called to get surface fog length
 * INPUT:         surf     surface
 * OUTPUT:        fog length
 *************/
float SurfaceGetFoglen(rsiCONTEXT *rc, SURFACE *surf)
{
	return surf->foglength;
}

/*************
 * DESCRIPTION:   called to set surface index of refraction
 * INPUT:         surf     surface
 *                ior      index of refraction
 * OUTPUT:        -
 *************/
void SurfaceIndOfRefr(rsiCONTEXT *rc, SURFACE *surf,float ior)
{
	surf->refrindex = ior;
}

/*************
 * DESCRIPTION:   called to set surface reflective phong factor
 * INPUT:         surf     surface
 *                refp     reflective phong
 * OUTPUT:        -
 *************/
void SurfaceRefPhong(rsiCONTEXT *rc, SURFACE *surf,float refp)
{
	surf->refphong = refp;
}

/*************
 * DESCRIPTION:   called to set surface tranparent phong factor
 * INPUT:         surf     surface
 *                transphong  transparent phong
 * OUTPUT:        -
 *************/
void SurfaceTransPhong(rsiCONTEXT *rc, SURFACE *surf, float transphong)
{
	surf->transphong = transphong;
}

/*************
 * DESCRIPTION:   called to set surface translucency factor
 * INPUT:         surf     surface
 *                transluc translucency
 * OUTPUT:        -
 *************/
void SurfaceTransluc(rsiCONTEXT *rc, SURFACE *surf, float transluc)
{
	surf->translucency = transluc;
}
#endif

#ifndef __PPC__
/*************
 * DESCRIPTION:   called to copy a surface
 * INPUT:         from     source
 *                to       destination
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL SurfaceCopy(rsiCONTEXT *rc, SURFACE *from,SURFACE *to)
{
	to = from->Duplicate();
	if(to)
	{
		to->SetName(from->name);
		to->Add();
		return TRUE;
	}

	return FALSE;
}

/*************
 * DESCRIPTION:   called to add a sphere
 * INPUT:         -
 * OUTPUT:        created sphere
 *************/
OBJECT *SphereAdd(rsiCONTEXT *rc)
{
	SPHERE *sphere;

	sphere = new SPHERE;
	if(!sphere)
		return NULL;

	sphere->selected = TRUE;
	sphere->external = EXTERNAL_CHILD;  // mark as external object

	sphere->OBJECT::Insert(where,dir);
	dir = INSERT_VERT;
	where = sphere;

	if(undo)
		undo->AddCreated(sphere);

	return (OBJECT*)sphere;
}

/*************
 * DESCRIPTION:   called to add triangles
 * INPUT:         count    amount of triangles
 *                surf     surface
 *                actor    actor
 * OUTPUT:        created triangles or NULL if failed
 *************/
TRIANGLE *TriangleAdd(rsiCONTEXT *rc, int count,SURFACE *surf,ACTOR *actor)
{
	MESH *mesh;

	mesh = new MESH;
	if(!mesh)
		return NULL;

	mesh->selected = TRUE;
	mesh->external = EXTERNAL_CHILD;    // mark as external child object

	mesh->OBJECT::Insert(where,dir);

	dir = INSERT_VERT;
	where = mesh;
	mesh->surf = surf;

	if(count)
	{
		if(mesh->AddTria(count) == -1)
			return NULL;
	}

	if(undo)
		undo->AddCreated(mesh);

	return mesh->trias;
}

/*************
 * DESCRIPTION:   called to add points to triangles
 * INPUT:         count    amount of points
 *                points   point vectors
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL TriangleAddPoints(rsiCONTEXT *rc, int count, VECTOR *points)
{
	int n,i;
	VECTOR *p;

	n = ((MESH*)where)->AddPoint(count);
	if(n == -1)
		return FALSE;

	p = &((MESH*)where)->points[n];
	for(i=n; i<n+count; i++)
	{
		// transform form absulute to relative coordinates
		*p = *points;
		stack->MultVectStack(p);

		p++;
		points++;
	}
	where->CalcBBox();
	return TRUE;
}

/*************
 * DESCRIPTION:   called to add edges to triangle
 * INPUT:         count    amount of edges
 *                edges    edge values
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL TriangleAddEdges(rsiCONTEXT *rc, int count, UWORD *edges)
{
	int n,i;
	EDGE *e;

	n = ((MESH*)where)->AddEdge(count);
	if(n == -1)
		return FALSE;

	e = &((MESH*)where)->edges[n];
	for(i=n; i<n+count; i++)
	{
		e->p[0] = *edges;
		edges++;
		e->p[1] = *edges;
		edges++;
		e++;
	}

	return TRUE;
}
#endif

#if !defined(__MIXEDBINARY__) || defined(__PPC__)
/*************
 * DESCRIPTION:   called to set edges of triangle
 * INPUT:         tria     triangle
 *                e1,e2,e3 edges
 * OUTPUT:        -
 *************/
void TriangleSetEdges(rsiCONTEXT *rc, TRIANGLE *tria,UWORD e1,UWORD e2,UWORD e3)
{
	tria->e[0] = e1;
	tria->e[1] = e2;
	tria->e[2] = e3;
}
#endif

#ifndef __PPC__
/*************
 * DESCRIPTION:   called to set flags of edges
 * INPUT:         flags    flags
 * OUTPUT:        -
 *************/
void TriangleSetEdgeFlags(rsiCONTEXT *rc, UBYTE *flags)
{
	int i;
	EDGE *e;

	e = ((MESH*)where)->edges;
	for(i=0; i<((MESH*)where)->actedges; i++)
	{
		e->flags = flags[i];
		e++;
	}
}
#endif

#if !defined(__MIXEDBINARY__) || defined(__PPC__)
/*************
 * DESCRIPTION:   called to set edge normals of triangle
 * INPUT:         tria     triangle
 *                v1,v2,v3 edge normals
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL TriangleVNorm(rsiCONTEXT *rc, TRIANGLE *tria,VECTOR *v1,VECTOR *v2,VECTOR *v3)
{
	return TRUE;
}

/*************
 * DESCRIPTION:   called to set the surface of a triangle
 * INPUT:         surf
 * OUTPUT:        none
 *************/
void TriangleSurface(rsiCONTEXT *rc, TRIANGLE *tria, SURFACE *surf)
{
}

/*************
 * DESCRIPTION:   called to get next triangle
 * INPUT:         tria     triangle
 * OUTPUT:        next triangle
 *************/
TRIANGLE * TriangleGetNext(rsiCONTEXT *rc, TRIANGLE *tria)
{
	return ++tria;
}
#endif

#ifndef __PPC__
/*************
 * DESCRIPTION:   called to create a mesh
 * INPUT:         -
 * OUTPUT:        created mesh
 *************/
OBJECT *MeshCreate(rsiCONTEXT *rc)
{
	MESH *mesh;

	mesh = new MESH;
	if(!mesh)
		return NULL;

	mesh->selected = TRUE;
	mesh->external = EXTERNAL_CHILD;

	mesh->OBJECT::Insert(where,dir);

	if(undo)
		undo->AddCreated(mesh);

	dir = INSERT_VERT;
	where = mesh;

	return (OBJECT*)mesh;
}

/*************
 * DESCRIPTION:   called to add a mesh
 * INPUT:         mesh        mesh
 *                points      mesh points
 *                pointcount  amount of points
 *                edges       mesh edges
 *                edgecount   amount of edges
 *                trias       mesh triangles
 *                triacount   amount of triangles
 *                nophong     disable phong shading
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL MeshAddScenario(rsiCONTEXT *rc, OBJECT *mesh, VECTOR *points, int pointcount,
	LINK_EDGE *edges, int edgecount, LINK_MESH *trias, int triacount,
	VECTOR *size, BOOL nophong)
{
	int i;
	VECTOR *p;
	TRIANGLE *t;
	LINK_MESH *lm;

	if(nophong)
		mesh->flags |= OBJECT_NOPHONG;

	if(triacount)
	{
		if(((MESH*)mesh)->AddTria(triacount) == -1)
			return FALSE;

		t = ((MESH*)mesh)->trias;
		lm = trias;
		for(i=0; i<triacount; i++)
		{
			t->e[0] = lm->e[0];
			t->e[1] = lm->e[1];
			t->e[2] = lm->e[2];
			t++;
			lm++;
		}
	}

	if(pointcount)
	{
		if(((MESH*)mesh)->AddPoint(pointcount) == -1)
			return FALSE;

		p = ((MESH*)mesh)->points;
		for(i=0; i<pointcount; i++)
		{
			// transform form absulute to relative coordinates
			*p = *points;
			p->x *= size->x;
			p->y *= size->y;
			p->z *= size->z;
			stack->MultVectStack(p);

			p++;
			points++;
		}
	}

	if(edgecount)
	{
		if(((MESH*)mesh)->AddEdge(edgecount) == -1)
			return FALSE;

		memcpy(((MESH*)mesh)->edges, edges, sizeof(EDGE)*edgecount);
	}

	return TRUE;
}

/*************
 * DESCRIPTION:   called to add a box
 * INPUT:         -
 * OUTPUT:        created box
 *************/
OBJECT *BoxAdd(rsiCONTEXT *rc)
{
	BOX *box;

	box = new BOX;
	if(!box)
		return NULL;

	box->selected = TRUE;
	box->external = EXTERNAL_CHILD;  // mark as external object

	box->OBJECT::Insert(where,dir);
	dir = INSERT_VERT;
	where = box;

	if(undo)
		undo->AddCreated(box);

	return (OBJECT*)box;
}
#endif

#if !defined(__MIXEDBINARY__) || defined(__PPC__)
/*************
 * DESCRIPTION:   called to set the box bounds
 * INPUT:         box
 *                lowbounds
 *                highbounds
 * OUTPUT:        -
 *************/
void BoxBounds(rsiCONTEXT *rc, OBJECT *box, VECTOR *lowbounds, VECTOR *highbounds)
{
	((BOX*)box)->bboxmin = *lowbounds;
	((BOX*)box)->bboxmax = *highbounds;
}
#endif

#ifndef __PPC__
/*************
 * DESCRIPTION:   called to add a cylinder
 * INPUT:         -
 * OUTPUT:        created cylinder
 *************/
OBJECT *CylinderAdd(rsiCONTEXT *rc)
{
	CYLINDER *cylinder;

	cylinder = new CYLINDER;
	if(!cylinder)
		return NULL;

	cylinder->selected = TRUE;
	cylinder->external = EXTERNAL_CHILD;  // mark as external object

	cylinder->OBJECT::Insert(where,dir);
	dir = INSERT_VERT;
	where = cylinder;

	if(undo)
		undo->AddCreated(cylinder);

	return (OBJECT*)cylinder;
}
#endif

#if !defined(__MIXEDBINARY__) || defined(__PPC__)
/*************
 * DESCRIPTION:   called to set cylinder flags
 * INPUT:         cylinder cylinder
 *                flags    flags
 * OUTPUT:        -
 *************/
void CylinderFlags(rsiCONTEXT *rc, OBJECT *cylinder, ULONG flags)
{
	if(flags & LINK_CYLINDER_OPENTOP)
		cylinder->flags |= OBJECT_OPENTOP;
	if(flags & LINK_CYLINDER_OPENBOTTOM)
		cylinder->flags |= OBJECT_OPENBOTTOM;
}
#endif

#ifndef __PPC__
/*************
 * DESCRIPTION:   called to add a cone
 * INPUT:         -
 * OUTPUT:        created cone
 *************/
OBJECT *ConeAdd(rsiCONTEXT *rc)
{
	CONE *cone;

	cone = new CONE;
	if(!cone)
		return NULL;

	cone->selected = TRUE;
	cone->external = EXTERNAL_CHILD;  // mark as external object

	cone->OBJECT::Insert(where,dir);
	dir = INSERT_VERT;
	where = cone;

	if(undo)
		undo->AddCreated(cone);

	return (OBJECT*)cone;
}
#endif

#if !defined(__MIXEDBINARY__) || defined(__PPC__)
/*************
 * DESCRIPTION:   called to set cone flags
 * INPUT:         cone     cone
 *                flags    flags
 * OUTPUT:        -
 *************/
void ConeFlags(rsiCONTEXT *rc, OBJECT *cone, ULONG flags)
{
	if(flags & LINK_CONE_OPENBOTTOM)
		cone->flags |= OBJECT_OPENBOTTOM;
}
#endif

#ifndef __PPC__
/*************
 * DESCRIPTION:   called to add a plane
 * INPUT:         -
 * OUTPUT:        created plane
 *************/
OBJECT *PlaneAdd(rsiCONTEXT *rc)
{
	PLANE *plane;

	plane = new PLANE;
	if(!plane)
		return NULL;

	plane->selected = TRUE;
	plane->external = EXTERNAL_CHILD;   // mark as external object

	plane->OBJECT::Insert(where,dir);
	dir = INSERT_VERT;
	where = plane;

	if(undo)
		undo->AddCreated(plane);

	return (OBJECT*)plane;
}

/*************
 * DESCRIPTION:   called to add a CSG object
 * INPUT:         -
 * OUTPUT:        created CSG object
 *************/
OBJECT *CSGAdd(rsiCONTEXT *rc)
{
	CSG *csg;

	csg = new CSG;
	if(!csg)
		return NULL;

	csg->selected = TRUE;
	csg->external = EXTERNAL_CHILD;   // mark as external object

	csg->OBJECT::Insert(where,dir);
	dir = INSERT_VERT;
	where = csg;

	if(undo)
		undo->AddCreated(csg);

	return (OBJECT*)csg;
}
#endif

#if !defined(__MIXEDBINARY__) || defined(__PPC__)
/*************
 * DESCRIPTION:   called to set CSG operation
 * INPUT:         csg      csg object
 *                operation
 * OUTPUT:        -
 *************/
void CSGOperation(rsiCONTEXT *rc, OBJECT *csg, UWORD operation)
{
	switch(operation)
	{
		case LINK_CSG_UNION:
			((CSG*)csg)->operation = CSG_UNION;
			break;
		case LINK_CSG_INTERSECTION:
			((CSG*)csg)->operation = CSG_INTERSECTION;
			break;
		case LINK_CSG_DIFFERENCE:
			((CSG*)csg)->operation = CSG_DIFFERENCE;
			break;
	}
}
#endif

#ifndef __PPC__
/*************
 * DESCRIPTION:   called to add a SOR object
 * INPUT:         -
 * OUTPUT:        created SOR object
 *************/
OBJECT *SORAdd(rsiCONTEXT *rc)
{
	SOR *sor;

	sor = new SOR;
	if(!sor)
		return NULL;

	sor->selected = TRUE;
	sor->external = EXTERNAL_CHILD;   // mark as external object

	sor->OBJECT::Insert(where,dir);
	dir = INSERT_VERT;
	where = sor;

	if(undo)
		undo->AddCreated(sor);

	return (OBJECT*)sor;
}
#endif

#if !defined(__MIXEDBINARY__) || defined(__PPC__)
/*************
 * DESCRIPTION:   called to set sor flags
 * INPUT:         sor      sor
 *                flags    flags
 * OUTPUT:        -
 *************/
void SORFlags(rsiCONTEXT *rc, OBJECT *sor, ULONG flags)
{
	if(flags & LINK_SOR_OPENTOP)
		sor->flags |= OBJECT_OPENTOP;
	if(flags & LINK_SOR_OPENBOTTOM)
		sor->flags |= OBJECT_OPENBOTTOM;
	if(flags & LINK_SOR_ACCURATE)
		sor->flags |= OBJECT_SOR_ACCURATE;
}
#endif

#ifndef __PPC__
/*************
 * DESCRIPTION:   called to set sor points
 * INPUT:         sor      sor
 *                points
 *                count    amount of points
 * OUTPUT:        FALSE if failed, else TRUE
 *************/
BOOL SORPoints(rsiCONTEXT *rc, OBJECT *sor, VECT2D *points, ULONG count)
{
	int i;

	((SOR*)sor)->FreeAllNodes();
	for(i = count-1; i >= 0; i--)
	{
		if(!((SOR*)sor)->AddNode(&points[i]))
			return FALSE;
	}
	return TRUE;
}

/*************
 * DESCRIPTION:   called to add an texture
 * INPUT:         name     texture name
 *                surf     surface
 *                actor    actor
 * OUTPUT:        created texture
 *************/
TEXTURE *ITextureAdd(rsiCONTEXT *rc, char *name,SURFACE *surf)
{
	TEXTURE *texture;

	texture = new IMAGINE_TEXTURE;
	if(!texture)
		return NULL;

	texture->SetName(name);

	// and add to surface texture list
	surf->AddTexture(texture);

	return texture;
}
#endif

#if !defined(__MIXEDBINARY__) || defined(__PPC__)
/*************
 * DESCRIPTION:   called to set texture parameters
 * INPUT:         texture     texture
 *                i           number
 *                param       value
 * OUTPUT:        -
 *************/
void ITextureParams(rsiCONTEXT *rc, TEXTURE *texture,int i,float param)
{
	((IMAGINE_TEXTURE*)texture)->params[i] = param;
}
#endif

#ifndef __PPC__
/*************
 * DESCRIPTION:   called to add an texture
 * INPUT:         name     texture name
 *                surf     surface
 *                actor    actor
 * OUTPUT:        created texture
 *************/
TEXTURE *RSTextureAdd(rsiCONTEXT *rc, char *name,SURFACE *surf)
{
	TEXTURE *texture;

	texture = new RAYSTORM_TEXTURE;
	if(!texture)
		return NULL;

	texture->SetName(name);

	// and add to surface texture list
	surf->AddTexture(texture);

	return texture;
}

/*************
 * DESCRIPTION:   called to set texture parameters
 * INPUT:         texture     texture
 *                size        size of parameter structure
 *                params      parameters
 * OUTPUT:        FLASE if failed
 *************/
BOOL RSTextureParams(rsiCONTEXT *rc, TEXTURE *texture, int size, UBYTE *params)
{
  ((RAYSTORM_TEXTURE*)texture)->datasize = size;
  ((RAYSTORM_TEXTURE*)texture)->data = new UBYTE[size];
  if(!((RAYSTORM_TEXTURE*)texture)->data)
	  return FALSE;

  memcpy(((RAYSTORM_TEXTURE*)texture)->data, params, size);
  return TRUE;
}
#endif

#ifndef __PPC__
/*************
 * DESCRIPTION:   called to add a texture axis definition to an object
 * INPUT:         obj      object
 *                actor    actor
 *                prev     previous created texture object (NULL for first)
 * OUTPUT:        created texture object
 *************/
TEXTURE_OBJECT *TextureObjectAdd(rsiCONTEXT *rc, OBJECT *obj, ACTOR *actor, TEXTURE_OBJECT *prev)
{
	TEXTURE_OBJECT *textureobj, *to;
	TEXTURE *t;

	textureobj = new TEXTURE_OBJECT;
	if(!textureobj)
		return NULL;

	textureobj->selected = TRUE;

	// and add to object texture list
	obj->AddTexture(textureobj);

	// search for the associated texture of the surface
	t = obj->surf->texture;
	to = obj->texture;
	while(t && to)
	{
		to = (TEXTURE_OBJECT*)to->GetNext();
		if(to)
			t = (TEXTURE*)t->GetNext();
	}
	textureobj->texture = t;

	return (TEXTURE_OBJECT*)textureobj;
}
#endif

#if !defined(__MIXEDBINARY__) || defined(__PPC__)
/*************
 * DESCRIPTION:   called to set texture position
 * INPUT:         texture     texture
 *                pos         position
 * OUTPUT:        -
 *************/
void TexturePos(rsiCONTEXT *rc, TEXTURE_OBJECT *texture,VECTOR *pos)
{
	texture->pos = *pos;
}

/*************
 * DESCRIPTION:   called to set texture orientation
 * INPUT:         texture     texture
 *                orient_x,orient_y,orient_z    orientation
 * OUTPUT:        -
 *************/
void TextureOrient(rsiCONTEXT *rc, TEXTURE_OBJECT *texture,VECTOR *orient_x,VECTOR *orient_y,VECTOR *orient_z)
{
	texture->orient_x = *orient_x;
	texture->orient_y = *orient_y;
	texture->orient_z = *orient_z;
}
#endif

#ifndef __PPC__
/*************
 * DESCRIPTION:   called to set texture size
 * INPUT:         texture     texture
 *                size        size
 * OUTPUT:        -
 *************/
void TextureSize(rsiCONTEXT *rc, TEXTURE_OBJECT *texture,VECTOR *size)
{
	texture->size = *size;
	texture->CalcBBox();
}

/*************
 * DESCRIPTION:   called to add a new brush, you have to call AddBrushObject too
 * INPUT:         name     brush name
 *                surf     surface
 * OUTPUT:        -
 *************/
BRUSH *BrushAdd(rsiCONTEXT *rc, char *name,SURFACE *surf)
{
	BRUSH *brush;

	brush = new BRUSH;
	if(!brush)
		return NULL;

	brush->SetName(name);

	// and add to surface brush list
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
BRUSH_OBJECT *BrushObjectAdd(rsiCONTEXT *rc, OBJECT *obj, ACTOR *actor, BRUSH_OBJECT *prev)
{
	BRUSH_OBJECT *brushobj, *bo;
	BRUSH *b;

	brushobj = new BRUSH_OBJECT;
	if(!brushobj)
		return NULL;

	brushobj->selected = TRUE;

	// and add to surface brush list
	obj->AddBrush(brushobj);

	// search for the associated brush of the surface
	b = obj->surf->brush;
	bo = obj->brush;
	while(b && bo)
	{
		bo = (BRUSH_OBJECT*)bo->GetNext();
		if(bo)
			b = (BRUSH*)b->GetNext();
	}
	brushobj->brush = b;

	return brushobj;
}
#endif

#if !defined(__MIXEDBINARY__) || defined(__PPC__)
/*************
 * DESCRIPTION:   called to set brush flags
 * INPUT:         brush    brush
 *                flags    flags
 * OUTPUT:        -
 *************/
void BrushFlags(rsiCONTEXT *rc, BRUSH *brush,ULONG flags)
{
	switch(flags & LINK_BRUSH_MAP_MASK)
	{
		default:
		case LINK_BRUSH_MAP_COLOR:
			brush->type = BRUSH_MAP_COLOR;
			break;
		case LINK_BRUSH_MAP_REFLECTIFITY:
			brush->type = BRUSH_MAP_REFLECTIVITY;
			break;
		case LINK_BRUSH_MAP_FILTER:
			brush->type = BRUSH_MAP_FILTER;
			break;
		case LINK_BRUSH_MAP_ALTITUDE:
			brush->type = BRUSH_MAP_ALTITUDE;
			break;
		case LINK_BRUSH_MAP_SPECULAR:
			brush->type = BRUSH_MAP_SPECULAR;
			break;
	}

	switch(flags & LINK_BRUSH_WRAPMASK)
	{
		case LINK_BRUSH_WRAPX:
			brush->wrap = BRUSH_WRAP_X;
			break;
		case LINK_BRUSH_WRAPY:
			brush->wrap = BRUSH_WRAP_Y;
			break;
		case LINK_BRUSH_WRAPXY:
			brush->wrap = BRUSH_WRAP_XY;
			break;
	}

	brush->flags = 0;
	if(flags & LINK_BRUSH_SOFT)
		brush->flags |= BRUSH_SOFT;
	if(flags & LINK_BRUSH_REPEAT)
		brush->flags |= BRUSH_REPEAT;
	if(flags & LINK_BRUSH_MIRROR)
		brush->flags |= BRUSH_MIRROR;
}
#endif

#ifndef __PPC__
/*************
 * DESCRIPTION:   called to set brush position
 * INPUT:         brush    brush
 *                pos      position
 * OUTPUT:        -
 *************/
void BrushPos(rsiCONTEXT *rc, BRUSH_OBJECT *brush,VECTOR *pos)
{
	brush->pos = *pos;
	stack->MultVectStack(&brush->pos);
}

/*************
 * DESCRIPTION:   called to set brush orientation
 * INPUT:         orient_x,orient_y,orient_z    orientation
 * OUTPUT:        -
 *************/
void BrushOrient(rsiCONTEXT *rc, BRUSH_OBJECT *brush,VECTOR *orient_x,VECTOR *orient_y,VECTOR *orient_z)
{
	MATRIX m, matrix;

	brush->orient_x = *orient_x;
	brush->orient_y = *orient_y;
	brush->orient_z = *orient_z;

	m.SetOMatrix(&brush->orient_x, &brush->orient_y, &brush->orient_z);
	matrix.MultMat(&m, &stack->matrix);
	SetVector(&brush->orient_x, matrix.m[5], matrix.m[9], matrix.m[13]);
	SetVector(&brush->orient_y, matrix.m[6], matrix.m[10], matrix.m[14]);
	SetVector(&brush->orient_z, matrix.m[7], matrix.m[11], matrix.m[15]);
}

/*************
 * DESCRIPTION:   called to set brush size
 * INPUT:         brush    brush
 *                size     size
 * OUTPUT:        -
 *************/
void BrushSize(rsiCONTEXT *rc, BRUSH_OBJECT *brush,VECTOR *size)
{
	brush->size = *size;
	brush->CalcBBox();
}
#endif

#if !defined(__MIXEDBINARY__) || defined(__PPC__)
/*************
 * DESCRIPTION:   called to set brush alignment
 * INPUT:         brush    brush
 *                algin    alignment
 * OUTPUT:        -
 *************/
void BrushAlignment(rsiCONTEXT *rc, BRUSH_OBJECT *brush,VECTOR *align)
{
}
#endif

#ifndef __PPC__
/*************
 * DESCRIPTION:   called to add a camera
 * INPUT:         -
 * OUTPUT:        created camera or NULL if failed
 *************/
CAMERA *CameraAdd(rsiCONTEXT *rc)
{
	CAMERA *camera;

	camera = new CAMERA;
	if(!camera)
		return NULL;

	camera->selected = TRUE;
	camera->external = EXTERNAL_CHILD;  // mark as external object

	camera->OBJECT::Insert(where,dir);
	dir = INSERT_VERT;
	where = (OBJECT*)camera;

	if(undo)
		undo->AddCreated(camera);

	return camera;
}
#endif

#if !defined(__MIXEDBINARY__) || defined(__PPC__)
/*************
 * DESCRIPTION:   called to set camera flags
 * INPUT:         camera      camera
 *                flags       flags
 * OUTPUT:        -
 *************/
void CameraFlags(rsiCONTEXT *rc, CAMERA *camera, ULONG flags)
{
	if(flags & LINK_CAMERA_VFOV)
		camera->flags |= OBJECT_CAMERA_VFOV;
	if(flags & LINK_CAMERA_FOCUSTRACK)
		camera->flags |= OBJECT_CAMERA_FOCUSTRACK;
	if(flags & LINK_CAMERA_FASTDOF)
		camera->flags |= OBJECT_CAMERA_FASTDOF;
}

/*************
 * DESCRIPTION:   called to set the focal distance of camera
 * INPUT:         camera      camera
 *                fdist       focal distance
 * OUTPUT:        -
 *************/
void CameraFocalDist(rsiCONTEXT *rc, CAMERA *camera, float fdist)
{
	camera->focaldist = fdist;
}

/*************
 * DESCRIPTION:   called to set camera aperture
 * INPUT:         camera      camera
 *                aperture    aperture
 * OUTPUT:        -
 *************/
void CameraAperture(rsiCONTEXT *rc, CAMERA *camera, float aperture)
{
	camera->aperture = aperture;
}

/*************
 * DESCRIPTION:   called to set camera field of view
 * INPUT:         camera      camera
 *                hfov, vfov  horizontal and vertical field of view
 * OUTPUT:        -
 *************/
void CameraFOV(rsiCONTEXT *rc, CAMERA *camera, float hfov, float vfov)
{
	camera->hfov = hfov;
	camera->vfov = vfov;
}

/*************
 * DESCRIPTION:   called to set the light color
 * INPUT:         light    light
 *                r,g,b    color
 * OUTPUT:        -
 *************/
void LightColor(rsiCONTEXT *rc, LIGHT *light, float r, float g, float b)
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
void LightFlags(rsiCONTEXT *rc, LIGHT *light, ULONG flags)
{
	if(flags & LINK_LIGHT_SHADOW)
		light->flags |= OBJECT_LIGHT_SHADOW;
	if(flags & LINK_LIGHT_FLARES)
		light->flags |= OBJECT_LIGHT_FLARES;
	if(flags & LINK_LIGHT_STAR)
		light->flags |= OBJECT_LIGHT_STAR;
	if(flags & LINK_LIGHT_SHADOWMAP)
		light->flags |= OBJECT_LIGHT_SHADOWMAP;
	if(flags & LINK_LIGHT_TRACKFALLOFF)
		light->flags |= OBJECT_LIGHT_TRACKFALLOFF;
}

/*************
 * DESCRIPTION:   called to set the light falloff value
 * INPUT:         light    light
 *                falloff  falloff value
 * OUTPUT:        -
 *************/
void LightFallOff(rsiCONTEXT *rc, LIGHT *light, float falloff)
{
	light->falloff = falloff;
}

/*************
 * DESCRIPTION:   called to set the light radius
 * INPUT:         light    light
 *                r        radius
 * OUTPUT:        -
 *************/
void LightRadius(rsiCONTEXT *rc, LIGHT *light, float r)
{
	light->r = r;
}

/*************
 * DESCRIPTION:   called to set the size of the shadowmap
 * INPUT:         light    light
 *                size     size of shadowmap
 * OUTPUT:        -
 *************/
void LightShadowMapSize(rsiCONTEXT *rc, LIGHT *light, int size)
{
	light->shadowmapsize = size;
}
#endif

#ifndef __PPC__
/*************
 * DESCRIPTION:   called to add a pointlight
 * INPUT:         -
 * OUTPUT:        created pointlight or NULL if failed
 *************/
LIGHT *PointLightAdd(rsiCONTEXT *rc)
{
	POINT_LIGHT *light;

	light = new POINT_LIGHT;
	if(!light)
		return NULL;

	light->selected = TRUE;
	light->external = EXTERNAL_CHILD;   // mark as external object

	light->OBJECT::Insert(where,dir);
	dir = INSERT_VERT;
	where = (OBJECT*)light;

	if(undo)
		undo->AddCreated(light);

	return light;
}

/*************
 * DESCRIPTION:   called to add a spotlight
 * INPUT:         -
 * OUTPUT:        created spotlight or NULL if failed
 *************/
LIGHT *SpotLightAdd(rsiCONTEXT *rc)
{
	SPOT_LIGHT *light;

	light = new SPOT_LIGHT;
	if(!light)
		return NULL;

	light->selected = TRUE;
	light->external = EXTERNAL_CHILD;   // mark as external object

	light->OBJECT::Insert(where,dir);
	dir = INSERT_VERT;
	where = (OBJECT*)light;

	if(undo)
		undo->AddCreated(light);

	return light;
}
#endif

#if !defined(__MIXEDBINARY__) || defined(__PPC__)
/*************
 * DESCRIPTION:   called to set the light opening angle
 * INPUT:         light    light
 *                angle    opening angle
 * OUTPUT:        -
 *************/
void SpotLightAngle(rsiCONTEXT *rc, LIGHT *light, float angle)
{
	((SPOT_LIGHT*)light)->angle = angle;
}

/*************
 * DESCRIPTION:   called to set the light falloff radius
 * INPUT:         light    light
 *                falloff  falloff radius
 * OUTPUT:        -
 *************/
void SpotLightFallOffRadius(rsiCONTEXT *rc, LIGHT *light, float falloff)
{
	((SPOT_LIGHT*)light)->falloff_radius = falloff;
}
#endif

#ifndef __PPC__
/*************
 * DESCRIPTION:   called to add a directional light
 * INPUT:         -
 * OUTPUT:        created directional light or NULL if failed
 *************/
LIGHT *DirectionalLightAdd(rsiCONTEXT *rc)
{
	DIRECTIONAL_LIGHT *light;

	light = new DIRECTIONAL_LIGHT;
	if(!light)
		return NULL;

	light->selected = TRUE;
	light->external = EXTERNAL_CHILD;   // mark as external object

	light->OBJECT::Insert(where,dir);
	dir = INSERT_VERT;
	where = (OBJECT*)light;

	if(undo)
		undo->AddCreated(light);

	return light;
}

/*************
 * DESCRIPTION:   called to add a flare to a light source
 * INPUT:         light    light to add flare to
 * OUTPUT:        created flare or NULL if failed
 *************/
FLARE *FlareAdd(rsiCONTEXT *rc, LIGHT *light)
{
	FLARE *flare;

	flare = new FLARE;
	if(!flare)
		return NULL;

	flare->Append((SLIST**)&light->flares);

	return flare;
}
#endif

#if !defined(__MIXEDBINARY__) || defined(__PPC__)
/*************
 * DESCRIPTION:   called to set the flare color
 * INPUT:         flare    pointer to current flare
 *                r,g,b    color
 * OUTPUT:        -
 *************/
void FlareColor(rsiCONTEXT *rc, FLARE *flare, float r, float g, float b)
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
void FlarePosition(rsiCONTEXT *rc, FLARE *flare, float pos)
{
	flare->pos = pos;
}

/*************
 * DESCRIPTION:   called to set the flare radius
 * INPUT:         flare    pointer to current flare
 *                r        radius
 * OUTPUT:        -
 *************/
void FlareRadius(rsiCONTEXT *rc, FLARE *flare, float r)
{
	flare->radius = r;
}

/*************
 * DESCRIPTION:   called to set the flare type
 * INPUT:         flare    pointer to current flare
 *                type     flare type
 * OUTPUT:        -
 *************/
void FlareType(rsiCONTEXT *rc, FLARE *flare, ULONG type)
{
	flare->type = type;
}

/*************
 * DESCRIPTION:   called to set the flare interpolation function
 * INPUT:         flare    pointer to current flare
 *                func     function
 * OUTPUT:        -
 *************/
void FlareFunction(rsiCONTEXT *rc, FLARE *flare, ULONG func)
{
	flare->func = func;
}

/*************
 * DESCRIPTION:   called to set the number of flare edges
 * INPUT:         flare    pointer to current flare
 *                num      number of edges
 * OUTPUT:        -
 *************/
void FlareEdges(rsiCONTEXT *rc, FLARE *flare, ULONG num)
{
	flare->edges = num;
}

/*************
 * DESCRIPTION:   called to set the flare tilt angle
 * INPUT:         flare    pointer to current flare
 *                tilt     tilt angle
 * OUTPUT:        -
 *************/
void FlareTilt(rsiCONTEXT *rc, FLARE *flare, float tilt)
{
	flare->tilt = tilt;
}
#endif

#ifndef __PPC__
/*************
 * DESCRIPTION:   called to add a star to a light source
 * INPUT:         light    light to add star to
 * OUTPUT:        created star or NULL if failed
 *************/
STAR *StarAdd(rsiCONTEXT *rc, LIGHT *light)
{
	if(light->star)
		delete light->star;

	light->star = new STAR;
	if(!light->star)
		return NULL;

	return light->star;
}
#endif

#if !defined(__MIXEDBINARY__) || defined(__PPC__)
/*************
 * DESCRIPTION:   called to set the star flags
 * INPUT:         star     pointer to current star
 *                flags    flags
 * OUTPUT:        -
 *************/
void StarFlags(rsiCONTEXT *rc, STAR *star, ULONG flags)
{
	if(flags & LINK_STAR_ENABLE)
		star->flags |= STAR_ENABLE;
	if(flags & LINK_STAR_HALOENABLE)
		star->flags |= STAR_HALOENABLE;
	if(flags & LINK_STAR_RANDOM)
		star->flags |= STAR_RANDOM;
}

/*************
 * DESCRIPTION:   called to set the star radius
 * INPUT:         star     pointer to current star
 *                r        radius
 * OUTPUT:        -
 *************/
void StarRadius(rsiCONTEXT *rc, STAR *star, float radius)
{
	star->starradius = radius;
}

/*************
 * DESCRIPTION:   called to set the star tilt angle
 * INPUT:         star     pointer to current star
 *                tilt     tilt angle
 * OUTPUT:        -
 *************/
void StarTilt(rsiCONTEXT *rc, STAR *star, float tilt)
{
	star->tilt = tilt;
}

/*************
 * DESCRIPTION:   called to set the amount of star spikes
 * INPUT:         star     pointer to current star
 *                spikes   amount of spikes
 * OUTPUT:        -
 *************/
void StarSpikes(rsiCONTEXT *rc, STAR *star, ULONG spikes)
{
	star->spikes = spikes;
}

/*************
 * DESCRIPTION:   called to set the halo radius of the star
 * INPUT:         star     pointer to current star
 *                r        halo radius
 * OUTPUT:        -
 *************/
void StarHaloradius(rsiCONTEXT *rc, STAR *star, float r)
{
	star->haloradius = r;
}

/*************
 * DESCRIPTION:   called to set the inner halo radius of the star
 * INPUT:         star     pointer to current star
 *                r        inner halo radius
 * OUTPUT:        -
 *************/
void StarInnerHaloradius(rsiCONTEXT *rc, STAR *star, float r)
{
	star->innerhaloradius = r;
}

/*************
 * DESCRIPTION:   called to set star spikes range
 * INPUT:         star     pointer to current star
 *                range    spikes range
 * OUTPUT:        -
 *************/
void StarRange(rsiCONTEXT *rc, STAR *star, float range)
{
	star->range = range;
}
#endif

#ifndef __PPC__
/*************
 * DESCRIPTION:   called to set the star intensities
 * INPUT:         star     pointer to current star
 *                num      amount of intensities
 *                inte     array of intensities
 * OUTPUT:        TRUE if all ok else FALSE
 *************/
BOOL StarIntensities(rsiCONTEXT *rc, STAR *star, int num, float *inte)
{
	if(star->intensities)
		delete star->intensities;

	star->intensities = new float[num];
	if(!star->intensities)
		return FALSE;

	memcpy(star->intensities, inte, sizeof(float)*num);

	return TRUE;
}
#endif

#if !defined(__MIXEDBINARY__) && !defined(__PPC__)
OBJLINK link =
{
	LINK_SCENARIO,

	ObjectBegin,
	ObjectEnd,
	ObjectSurface,
	ObjectName,
	ObjectAxis,
	NULL,             // ObjectActor
	ObjectFlags,
	ObjectTrack,

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
	NULL,             // SphereAxis

	TriangleAdd,
	TriangleAddPoints,
	TriangleAddEdges,
	TriangleSetEdges,
	TriangleSetEdgeFlags,
	NULL,             // TrianglePoints
	TriangleVNorm,
	NULL,             // TriangleMapping
	TriangleSurface,
	TriangleGetNext,

	MeshCreate,
	MeshAddScenario,
	NULL,             // MeshAddRenderer

	BoxAdd,
	BoxBounds,
	NULL,             // BoxPos

	CylinderAdd,
	NULL,             // CylinderAxis
	CylinderFlags,

	ConeAdd,
	NULL,             // ConeAxis
	ConeFlags,

	PlaneAdd,
	NULL,             // PlaneAxis

	CSGAdd,
	CSGOperation,

	SORAdd,
	NULL,             // SORAxis
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

	CameraAdd,
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
#endif // !__MIXEDBINARY__ || __PPC__
