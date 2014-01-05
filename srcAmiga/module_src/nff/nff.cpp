/*************
 * MODULE:        NFF-object format handler
 * NAME:          nff.cpp
 * PROJECT:       RayStorm
 * DESCRIPTION:   This module keeps functions to read nff format-files
 * AUTHORS:       Mike Hesser, Andreas Heumann
 * TO DO:         elimination of double edges
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    13.06.98 ah    first release version
 *************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#ifdef __AMIGA__
	#include <exec/memory.h>
	#include <pragma/iffparse_lib.h>
	#include <pragma/dos_lib.h>
	#include <pragma/exec_lib.h>
	#include "rmod:objlib.h"
	#pragma libbase ObjHandBase
	#include <clib/powerpc_protos.h>
#else
	#include <malloc.h>
#endif

#ifdef __PPC__
#define SAVEDS __saveds
#else
#define SAVEDS
#endif

#ifndef VECMATH_H
#include "vecmath.h"
#endif

#ifndef COLOR_H
#include "color.h"
#endif

class ACTOR;
class SURFACE;
class TRIANGLE;
class SPHERE;
class BOX;
class CYLINDER;
class CONE;
class PLANE;
class TEXTURE;
class TEXTURE_OBJECT;
class BRUSH;
class BRUSH_OBJECT;
class CAMERA;
class LIGHT;
class POINT_LIGHT;
class SPOT_LIGHT;
class FLARE;
class STAR;
class OBJECT;
typedef void rsiCONTEXT;

#ifndef OBJLINK_H
#include "objlink.h"
#endif

typedef FILE NFFHandle;

#ifndef NFF_H
#include "nff.h"
#endif

enum errnums
{
	OK, ERR_MEM, ERR_NONFFFILE, ERR_OPEN, ERR_READ
};

static char *errors[] =
{
	NULL,
	"Not a valid NFF file",
	"Not enough memory",
	"Can't open file",
	"Error reading file",
};

// local variables
typedef struct
{
	NFFHandle  *hFile;
	MATRIX     transmatrix;
	ACTOR      *mainactor;
	OBJLINK    *link;
	rsiCONTEXT *rc;
	VECTOR pos, ox,oy,oz, size;
} HANDLER_DATA;

extern "C" void Cleanup(HANDLER_DATA*);

/*************
 * DESCRIPTION:   Init object handler
 * INPUT:         none
 * OUTPUT:        FALSE if failed
 *************/
extern "C" BOOL SAVEDS objInit()
{
	return TRUE;
}

/*************
 * DESCRIPTION:   open a NFF file
 * INPUT:         filename    name of file
 * OUTPUT:        NULL when failed else filehandle
 *************/
static NFFHandle *OpenNFF(HANDLER_DATA *data, const char *filename)
{
	return data->hFile = fopen(filename, "rb");
}


/*************
 * DESCRIPTION:   close NFF file
 * INPUT:         -
 * OUTPUT:        -
 *************/
static void CloseNFF(HANDLER_DATA *data)
{
	fclose(data->hFile);
}

/*************
 * DESCRIPTION:   Search the edge for three given points. If no edge is found, create
 *    a new one.
 * INPUT:         face        current face
 *                edgecount   current amount of edges
 *                point1      first point
 *                point2      second point
 *                point3      third point
 *                edges       edge array
 * OUTPUT:        -
 *************/
static void FindEdges(LINK_MESH *face, ULONG *edgecount, UWORD point1, UWORD point2, UWORD point3, LINK_EDGE *edges)
{
	BOOL found1, found2, found3;
	ULONG j;
	LINK_EDGE *e;

	found1 = FALSE;
	found2 = FALSE;
	found3 = FALSE;
	e = edges;
	for(j=0; (j<*edgecount) && (!found1 || !found2 || !found3); j++)
	{
		if(e->p[0] == point1)
		{
			if(e->p[1] == point2)
			{
				face->e[0] = j;
				found1 = TRUE;
			}
			else
			{
				if(e->p[1] == point3)
				{
					face->e[2] = j;
					found3 = TRUE;
				}
			}
		}
		if(e->p[0] == point2)
		{
			if(e->p[1] == point1)
			{
				face->e[0] = j;
				found1 = TRUE;
			}
			else
			{
				if(e->p[1] == point3)
				{
					face->e[1] = j;
					found2 = TRUE;
				}
			}
		}
		if(e->p[0] == point3)
		{
			if(e->p[1] == point2)
			{
				face->e[1] = j;
				found2 = TRUE;
			}
			else
			{
				if(e->p[1] == point1)
				{
					face->e[0] = j;
					found3 = TRUE;
				}
			}
		}
		e++;
	}
	if(!found1)
	{
		edges[*edgecount].p[0] = point1;
		edges[*edgecount].p[1] = point2;
		edges[*edgecount].flags = 0;
		face->e[0] = *edgecount;
		(*edgecount)++;
	}
	if(!found2)
	{
		edges[*edgecount].p[0] = point2;
		edges[*edgecount].p[1] = point3;
		edges[*edgecount].flags = 0;
		face->e[1] = *edgecount;
		(*edgecount)++;
	}
	if(!found3)
	{
		edges[*edgecount].p[0] = point3;
		edges[*edgecount].p[1] = point1;
		edges[*edgecount].flags = 0;
		face->e[2] = *edgecount;
		(*edgecount)++;
	}
}

/*************
 * DESCRIPTION: read all triangles of file
 * INPUT:       surf    surface to replace object surface with
 * OUTPUT:      -
 *************/
static char *ReadTriangles(HANDLER_DATA *data, SURFACE *surf)
{
	SURFACE *newsurf;
	char buf[256], name[256];
	ULONG tricount;
	ULONG pointcount, edgecount, curpoint, curtria, count;
	VECTOR *points;
	LINK_EDGE *edges;
	LINK_MESH *triangles;
	ULONG p0,p1,p2;
	OBJECT *mesh;

	edgecount = 0;

	newsurf = surf;

	fgets(buf, 256, data->hFile);
	if(!strcmp(buf, "nff"))
		return errors[ERR_NONFFFILE];

	// skip version
	fgets(buf, 256, data->hFile);
	// read name
	fgets(name, 256, data->hFile);
	if(!newsurf)
	{
		newsurf = data->link->SurfaceAdd(data->rc);
		if(!newsurf)
			return errors[ERR_MEM];
		if(!data->link->SurfaceName(data->rc, newsurf, name))
			return errors[ERR_MEM];
	}
	// read point count
	fgets(buf, 256, data->hFile);
	sscanf(buf, "%d", &pointcount);
	if(pointcount <= 0)
		return errors[ERR_NONFFFILE];

	points = new VECTOR[pointcount];
	if(!points)
		return errors[ERR_MEM];

	curpoint = 0;
	while (!feof(data->hFile) && (curpoint < pointcount))
	{
		fgets(buf, 256, data->hFile);

		if(sscanf(buf, "%f %f %f", &points[curpoint].x, &points[curpoint].y, &points[curpoint].z) != 3)
			return errors[ERR_NONFFFILE];

		curpoint++;
	}

	// read triangle count
	fgets(buf, 256, data->hFile);
	sscanf(buf, "%d", &tricount);

	if(tricount <= 0)
		return errors[ERR_NONFFFILE];

	triangles = new LINK_MESH[tricount];
	if(!triangles)
		return errors[ERR_MEM];

	edges = new LINK_EDGE[tricount*3];
	if(!edges)
		return errors[ERR_MEM];

	curtria = 0;
	while (!feof(data->hFile) && (curtria < tricount))
	{
		fgets(buf, 256, data->hFile);

		if(sscanf(buf, "%d %d %d %d", &count, &p0, &p1, &p2) != 4)
			return errors[ERR_NONFFFILE];

		FindEdges(&triangles[curtria], &edgecount, p0, p1, p2, edges);

		curtria++;
	}

	data->link->ObjectBegin(data->rc);
	if (data->link->type == LINK_SCENARIO)
	{
		mesh = data->link->MeshCreate(data->rc);
		if(!mesh)
			return errors[ERR_MEM];

		data->link->ObjectName(data->rc, mesh, name);
		data->link->ObjectSurface(data->rc, mesh, newsurf);

		if(!data->link->MeshAddScenario(data->rc, mesh,
			points, pointcount,
			edges, edgecount,
			triangles, tricount,
			&data->size,
			FALSE))
		{
			return errors[ERR_MEM];
		}
	}
	else
	{
		if(!data->link->MeshAddRenderer(data->rc, newsurf, NULL, data->mainactor,
			points, pointcount,
			edges, edgecount,
			triangles, tricount,
			&data->pos, &data->ox, &data->oy, &data->oz, &data->size,
			FALSE))
		{
			return errors[ERR_MEM];
		}
	}
	data->link->ObjectEnd(data->rc);

	delete points;
	delete edges;
	delete triangles;

	return NULL;
}

/*************
 * DESCRIPTION: read a NFF-file
 * INPUT:       rc        context
 *              filename  name of NFF-file
 *              link      link structure
 *              pos       object position
 *              ox,oy,oz  object orientation
 *              actor     pointer to actor
 *              surf      surface to replace object surface with
 *              version   version number
 * OUTPUT:      returns error string (NULL if ok)
 *************/
extern "C" char * SAVEDS objRead(rsiCONTEXT *rc, char *filename, OBJLINK *link, const VECTOR *pos, const VECTOR *ox, const VECTOR *oy, const VECTOR *oz,
	const VECTOR *scale, ACTOR *actor, SURFACE *surf, ULONG *version)
{
	char *errstr;
	HANDLER_DATA data;

	data.link = link;
	data.mainactor = actor;
	data.rc = rc;
	data.hFile = NULL;
	data.pos = *pos;
	data.ox = *ox;
	data.oy = *oy;
	data.oz = *oz;
	data.size = *scale;

	if (!OpenNFF(&data, filename))
	{
		Cleanup(&data);
		return errors[ERR_OPEN];
	}

	errstr = ReadTriangles(&data, surf);

	Cleanup(&data);
	return errstr;
}


/*************
 * DESCRIPTION: cleanup all allocated resources
 * INPUT:       -
 * OUTPUT:      -
 *************/
void Cleanup(HANDLER_DATA *data)
{
	if (data->hFile)
	{
		CloseNFF(data);
		data->hFile = NULL;
	}
}

/*************
 * DESCRIPTION:   cleanup all allocated resources
 * INPUT:       -
 * OUTPUT:      -
 *************/
extern "C" void SAVEDS objCleanup()
{
}

