/*************
 * MODULE:        RAW-object format handler
 * NAME:          raw.cpp
 * PROJECT:       RayStorm
 * DESCRIPTION:   This module keeps functions to read raw format-files
 * AUTHORS:       Mike Hesser, Andreas Heumann
 * TO DO:         elimination of double edges
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    13.03.96 mh    first release version
 *    11.06.96 mh    new version for PC & Amiga
 *    11.02.97 ah    added support for scenario
 *    15.05.97 ah    added version
 *    25.06.97 ah    changed to new V2 style
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

#ifndef LIST_H
#include "list.h"
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

typedef FILE RAWHandle;

#ifndef RAW_H
#include "raw.h"
#endif

enum errnums
{
	OK, ERR_MEM, ERR_OPEN, ERR_READ
};

static char *errors[] =
{
	NULL,
	"Not enough memory",
	"Can't open RAW file",
	"Error reading RAW file",
};

// local variables
typedef struct
{
	RAWHandle  *hFile;
	MATRIX     transmatrix;
	ACTOR      *mainactor;
	TRIA       *root;
	OBJLINK    *link;
	rsiCONTEXT *rc;
} HANDLER_DATA;

extern "C" void objCleanup(HANDLER_DATA*);

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
 * DESCRIPTION:   open a RAW file
 * INPUT:         filename    name of file
 * OUTPUT:        NULL when failed else filehandle
 *************/
static RAWHandle *OpenRAW(HANDLER_DATA *data, const char *filename)
{
	return data->hFile = fopen(filename, "rb");
}


/*************
 * DESCRIPTION:   close RAW file
 * INPUT:         -
 * OUTPUT:        -
 *************/
static void CloseRAW(HANDLER_DATA *data)
{
	fclose(data->hFile);
}

/*************
 * DESCRIPTION: transfer the buffered triangles to interface
 * INPUT:       tricount   amount of triangles
 *              surf       surface for this object
 * OUTPUT:      -
 *************/
char *Takeover(HANDLER_DATA *data, int tricount, SURFACE *surf)
{
	VECTOR *points, *p;
	EDGE *edges, *e;
	TRIA *cur;
	TRIANGLE *triangle;
	int tria;

	// generate new object
	triangle = data->link->TriangleAdd(data->rc, tricount, surf, data->mainactor);
	if (!triangle)
		return errors[ERR_MEM];

	// the modeler needs points,faces and edges; RayStorm only
	// the triangles
	if (data->link->type == LINK_SCENARIO)
	{
		points = new VECTOR[tricount*3];
		if(!points)
			return errors[ERR_MEM];

		edges = new EDGE[tricount*3];
		if(!edges)
		{
			delete points;
			return errors[ERR_MEM];
		}

		cur = data->root;
		tria = 0;
		p = points;
		e = edges;
		while(cur)
		{
			*p = cur->points[0];
			p++;
			*p = cur->points[1];
			p++;
			*p = cur->points[2];
			p++;

			e->begin = tria;
			e->end = tria+1;
			e++;

			e->begin = tria+1;
			e->end = tria+2;
			e++;

			e->begin = tria+2;
			e->end = tria;
			e++;

			tria += 3;
			cur = (TRIA*)cur->GetNext();
		}

		if(!data->link->TriangleAddPoints(data->rc, 3*tricount, points))
			return errors[ERR_MEM];

		delete points;

		if(!data->link->TriangleAddEdges(data->rc, 3*tricount, (UWORD*)edges))
			return errors[ERR_MEM];

		delete edges;

		for(tria=0; tria<tricount; tria++)
		{
			data->link->TriangleSetEdges(data->rc, triangle, tria*3, tria*3+1, tria*3+2);
			triangle = data->link->TriangleGetNext(data->rc, triangle);
		}
	}
	else // raytracer
	{
		triangle = data->link->TriangleAdd(data->rc, tricount, surf, data->mainactor);
		if(!triangle)
			return errors[ERR_MEM];

		cur = data->root;
		while(cur)
		{
			data->link->TrianglePoints(data->rc, triangle, &cur->points[0], &cur->points[1], &cur->points[2]);
			cur = (TRIA*)cur->GetNext();
			triangle = data->link->TriangleGetNext(data->rc, triangle);
		}
	}

	// delete bufferd triangles
	data->root->FreeList();
	data->root = NULL;

	return NULL;
}

/*************
 * DESCRIPTION: read all triangles of file
 * INPUT:       surf    surface to replace object surface with
 * OUTPUT:      -
 *************/
static char *ReadTriangles(HANDLER_DATA *data, SURFACE *surf)
{
	TRIA *tri;
	SURFACE *newsurf;
	char buf[256];
	char *pos, *errstr;
	int tricount;
	int pointcount, edgecount;

	pointcount = 0;
	edgecount  = 0;
	tricount = 0;

	newsurf = surf;

	while (!feof(data->hFile))
	{
		fgets(buf, 256, data->hFile);
		pos = buf;

		// skip spaces
		while (*pos == ' ')
			pos++;

		// read triangle if float else skip identifier
		if (!(isdigit(pos[0]) || pos[0] == '.' || pos[0] == '+' || pos[0] == '-'))
		{
			if(!newsurf)
			{
				newsurf = data->link->SurfaceAdd(data->rc);
				if(!newsurf)
					return errors[ERR_MEM];
				if(!data->link->SurfaceName(data->rc, newsurf, buf))
					return errors[ERR_MEM];
			}

			if (tricount > 0)
			{
				errstr = Takeover(data, tricount, newsurf);
				if(errstr)
					return errstr;
				tricount = 0;

				newsurf = surf;
			}

			if(!newsurf)
			{
				newsurf = data->link->SurfaceAdd(data->rc);
				if(!newsurf)
					return errors[ERR_MEM];
				if(!data->link->SurfaceName(data->rc, newsurf, buf))
					return errors[ERR_MEM];
			}

		}
		else
		{
			tri = new TRIA;
			if (!tri)
				return errors[ERR_MEM];

			// read vertice data
			sscanf(buf, "%f %f %f %f %f %f %f %f %f",
				&tri->points[0].x, &tri->points[0].y, &tri->points[0].z,
				&tri->points[1].x, &tri->points[1].y, &tri->points[1].z,
				&tri->points[2].x, &tri->points[2].y, &tri->points[2].z);

			// transform the points
			data->transmatrix.MultVectMat(&tri->points[0]);
			data->transmatrix.MultVectMat(&tri->points[1]);
			data->transmatrix.MultVectMat(&tri->points[2]);

			tricount++;
			tri->Insert((LIST **)&data->root);
		}
	}

	if(tricount>0)
	{
		if(!newsurf)
		{
			newsurf = data->link->SurfaceAdd(data->rc);
			if(!newsurf)
				return errors[ERR_MEM];
			if(!data->link->SurfaceName(data->rc, newsurf, buf))
				return errors[ERR_MEM];
		}

		errstr = Takeover(data, tricount, newsurf);
		if(errstr)
			return errstr;
	}

	return NULL;
}

/*************
 * DESCRIPTION: read a RAW-file
 * INPUT:       rc        context
 *              filename  name of RAW-file
 *              link      link structure
 *              pos       object position
 *              ox,oy,oz  object orientation
 *              actor     pointer to actor
 *              surf      surface to replace object surface with
 *              version   version number
 * OUTPUT:      returns error string (NULL if ok)
 *************/
extern "C" char* SAVEDS objRead(rsiCONTEXT *rc, char *filename, OBJLINK *link, const VECTOR *pos, const VECTOR *ox, const VECTOR *oy, const VECTOR *oz,
	const VECTOR *scale, ACTOR *actor, SURFACE *surf, ULONG *version)
{
	char *errstr;
	HANDLER_DATA data;

	data.link = link;
	data.mainactor = actor;
	data.rc = rc;
	data.root = NULL;

	data.transmatrix.IdentityMatrix();
	data.transmatrix.SetSOTMatrix(scale, ox, oy, oz, pos);

	if (!OpenRAW(&data, filename))
	{
		objCleanup(&data);
		return errors[ERR_OPEN];
	}

	errstr = ReadTriangles(&data, surf);

	objCleanup(&data);
	return errstr;
}


/*************
 * DESCRIPTION:   cleanup all allocated resources
 * INPUT:       -
 * OUTPUT:      -
 *************/
extern "C" void SAVEDS objCleanup(HANDLER_DATA *data)
{
	if (data->hFile)
	{
		CloseRAW(data);
		data->hFile = NULL;
	}
	if(data->root)
	{
		data->root->FreeList();
		data->root = NULL;
	}
}
