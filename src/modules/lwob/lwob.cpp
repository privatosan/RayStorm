/***************
 * MODULE:        LWOB handler (Lightwave Object File Format)
 * NAME:          lwob.cpp
 * DESCRIPTION:   This module keeps all functions to read LWOB-files
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * TODO:          Sub-surfaces for polygons
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    24.09.97 ah    initial release
 *    26.06.99 ah    now can read also polygons not only triangles and rectangles
 *    27.06.99 ah    read glossiness as specular exponent
 ***************/

// TODO: not always all points for all objects
//       don't scan all faces everytime for each object
//       don't do edge optimization code for renderer

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef __AMIGA__
	#include "iffparse.h"
#else
	#include <exec/libraries.h>
	#include <utility/tagitem.h>
	#include <libraries/iffparse.h>
	#include <exec/memory.h>
	#include <pragma/iffparse_lib.h>
	#include <pragma/dos_lib.h>
	#include <pragma/exec_lib.h>
	#include "rmod:objlib.h"
	#pragma libbase ObjHandBase
	#include <clib/powerpc_protos.h>
#endif

#ifdef __PPC__
#define SAVEDS __saveds
#else
#define SAVEDS
#endif

#ifndef VECMATH_H
#include "vecmath.h"
#endif

#ifndef LWOB_H
#include "lwob.h"
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

enum errnums
{
	ERR_MEM,ERR_OPENFILE,ERR_LWOBFILE
};

class SURF_DATA
{
	public:
		SURFACE *surface;
		ULONG object_count;
		OBJECT **objects;
		char *name;
		BOOL nobound;
		VECTOR lowbound, highbound;
		ULONG facecount;
		ULONG edgecount;

		SURF_DATA();
		~SURF_DATA();
};

SURF_DATA::SURF_DATA()
{
	nobound = TRUE;
	SetVector(&lowbound, INFINITY, INFINITY, INFINITY);
	SetVector(&highbound, -INFINITY, -INFINITY, -INFINITY);
	facecount = 0;
	edgecount = 0;
	name = NULL;
	object_count = 0;
	objects = NULL;
}

SURF_DATA::~SURF_DATA()
{
	if(name)
		delete name;

	if(objects)
		delete objects;
}

typedef struct
{
	OBJLINK *link;
	VECTOR pos,ox,oy,oz,size;
	IFFHandle *iff;
	ULONG pointcount;
	VECTOR *points;
	rsiCONTEXT *rc;
	ULONG surfcount;
	SURF_DATA *surfaces;
	SURFACE *replacesurf;
	ULONG size_done, filesize;
	void (*SetProgress)(rsiCONTEXT*, float);
} HANDLER_DATA;

static char *errors[] =
{
	"Not enough memory",
	"Can't open LWOB object file",
	"Corrupt LWOB file"
};

static void readLWOB_cleanup(HANDLER_DATA*);

#define READ_COLOR(buffer, r,g,b) \
	(r) = ((float)buffer[cur]) / 255.f; \
	(g) = ((float)buffer[cur+1]) / 255.f; \
	(b) = ((float)buffer[cur+2]) / 255.f;

#ifdef __SWAP__
#define READULONG(cur) \
	(ULONG)buffer[cur] + (((ULONG)buffer[cur+1] + (((ULONG)buffer[cur+2] + ((ULONG)buffer[cur+3] << 8)) << 8)) << 8)
#else
#define READULONG(cur) \
	(ULONG)buffer[cur+3] + (((ULONG)buffer[cur+2] + (((ULONG)buffer[cur+1] + ((ULONG)buffer[cur] << 8)) << 8)) << 8)
#endif

#ifdef __SWAP__
#define READUWORD(cur) \
	(UWORD)buffer[cur] + ((UWORD)buffer[cur+1] << 8)
#else
#define READUWORD(cur) \
	(UWORD)buffer[cur+1] + ((UWORD)buffer[cur] << 8)
#endif

/*#ifdef __PPC__
#define ALLOCMEM(a,b) (a*)AllocVecPPC(b*sizeof(a), MEMF_PUBLIC,0)
#define FREEMEM(a) FreeVecPPC(a)
#else*/
#define ALLOCMEM(a,b) new a[b]
#define FREEMEM(a) delete a
//#endif

#define FLAGS_COLOR_HIGHLIGHTS   0x08
#define FLAGS_COLOR_FILTER       0x10

enum textures
{
	TEXTURE_NONE,
	TEXTURE_DIFFUSE,
	TEXTURE_SPECULAR,
	TEXTURE_REFLECTION,
	TEXTURE_TRANSPARENCY,
	TEXTURE_BUMP
};

#define FLAGS_TEXTURE_XAXIS      0x0001
#define FLAGS_TEXTURE_YAXIS      0x0002
#define FLAGS_TEXTURE_ZAXIS      0x0004
#define FLAGS_TEXTURE_WORLD      0x0008
#define FLAGS_TEXTURE_NEGATIVE   0x0010
#define FLAGS_TEXTURE_BLEND      0x0020
#define FLAGS_TEXTURE_ANTIALIAS  0x0040

#define TEXTURE_REPEAT           0x0002

/*************
 * DESCRIPTION:   Init object handler
 * INPUT:         -
 * OUTPUT:        FALSE if failed
 *************/
extern "C" BOOL SAVEDS objInit()
{
	return TRUE;
}

/*************
 * DESCRIPTION:   Read word from iff-file
 * INPUT:         h        iffhandle
 *                buf      pointer to buffer
 *                cnt      number of words
 * OUTPUT:        FALSE if failed, else TRUE
 *************/
static BOOL ReadWord(IFFHandle *h, WORD *buf, int cnt)
{
#ifdef __SWAP__
	BOOL res;
	int  i;

	res = (ReadChunkBytes(h, buf, sizeof(WORD) * cnt) == sizeof(WORD) * cnt);
	for(i = 0; i < cnt; i++)
		buf[i] = SwapW(buf[i]);
	return res;
#else
	return (ReadChunkBytes(h, buf, sizeof(WORD) * cnt) == sizeof(WORD) * cnt);
#endif
}

/*************
 * DESCRIPTION:   Read floats from iff-file
 * INPUT:         h        iffhandle
 *                buf      pointer to buffer
 *                cnt      number of floats
 * OUTPUT:        FALSE if failed, else TRUE
 *************/
static BOOL ReadFloat(IFFHandle *h, float *buf, int cnt)
{
	if (!ReadChunkBytes(h, buf, sizeof(float) * cnt) == sizeof(float) * cnt)
		return FALSE;

#ifdef __SWAP__
	for(int i = 0; i < cnt; i++)
		buf[i] = float(LONG(Swap(buf[i])));
#endif

	return TRUE;
}

/*************
 * DESCRIPTION:   free temporrary buffers
 * INPUT:         data     handler data
 * OUTPUT:        none
 *************/
static void FreeBuffers(HANDLER_DATA *data)
{
	if(data->points)
	{
		FREEMEM(data->points);
		data->points = NULL;
	}
	if(data->surfaces)
	{
		FREEMEM(data->surfaces);
		data->surfaces = NULL;
	}
}

/*************
 * FUNCTION:      OpenIFF
 * DESCRIPTION:   open iff file and parse to begin of file
 * INPUT:         iff      iff handle
 *                filename name of file
 *                filesize   size of file is returned in this variable
 * OUTPUT:        error string
 *************/
static char *OpenIFF(struct IFFHandle **iff, char *filename, ULONG *filesize)
{
	struct ContextNode *cn;

	// Allocate IFF_File structure.
	*iff = AllocIFF();
	if(!*iff)
		return errors[ERR_MEM];

#ifdef __AMIGA__
	// Set up IFF_File for AmigaDOS I/O.
	(*iff)->iff_Stream = Open(filename, MODE_OLDFILE);
#else
	(*iff)->iff_Stream = Open_(filename, MODE_OLDFILE);
#endif
	if(!(*iff)->iff_Stream)
		return errors[ERR_OPENFILE];

	InitIFFasDOS(*iff);

	// Start the IFF transaction.
	if(OpenIFF(*iff, IFFF_READ))
		return errors[ERR_LWOBFILE];

	if(ParseIFF(*iff, IFFPARSE_RAWSTEP))
		return errors[ERR_LWOBFILE];

	cn = CurrentChunk(*iff);
	if((cn->cn_ID != ID_FORM) || (cn->cn_Type != ID_LWOB))
		return errors[ERR_LWOBFILE];

	*filesize = cn->cn_Size;

	return NULL;
}

/*************
 * DESCRIPTION:   read the points
 * INPUT:         data     handler data
 *                cn       iff context node
 * OUTPUT:        error string
 *************/
static char *ReadPoints(HANDLER_DATA *data, struct ContextNode *cn)
{
	int count;

	// get point count (each point has a size of 12 bytes)
	count = (cn->cn_Size)/sizeof(VECTOR);

	// Allocate memory
	data->pointcount = count;
	data->points = ALLOCMEM(VECTOR, count);
	if (!data->points)
		return errors[ERR_MEM];

	// Read points
	if(!ReadFloat(data->iff, (float*)data->points, count*3))
		return errors[ERR_LWOBFILE];

	return NULL;
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
	int j;
	LINK_EDGE *e;

	found1 = FALSE;
	found2 = FALSE;
	found3 = FALSE;
	e = &edges[*edgecount - 1];
	for(j = *edgecount - 1; j >= 0 && (!found1 || !found2 || !found3); j--)
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
		e--;
	}
	if(!found1)
	{
		edges[*edgecount].p[0] = point1;
		edges[*edgecount].p[1] = point2;
		edges[*edgecount].flags = 0;
		face->e[0] = (UWORD)*edgecount;
		(*edgecount)++;
	}
	if(!found2)
	{
		edges[*edgecount].p[0] = point2;
		edges[*edgecount].p[1] = point3;
		edges[*edgecount].flags = 0;
		face->e[1] = (UWORD)*edgecount;
		(*edgecount)++;
	}
	if(!found3)
	{
		edges[*edgecount].p[0] = point3;
		edges[*edgecount].p[1] = point1;
		edges[*edgecount].flags = 0;
		face->e[2] = (UWORD)*edgecount;
		(*edgecount)++;
	}
}

/*************
 * DESCRIPTION:   read the polygons
 * INPUT:         data     handler data
 *                cn       iff context node
 *                actor    actor
 * OUTPUT:        error string
 *************/
static char *ReadPolygons(HANDLER_DATA *data, struct ContextNode *cn, ACTOR *actor)
{
	ULONG count, edgecount, i,j,k,l;
	WORD *polybuffer = NULL, *p;
	LINK_EDGE *edges;
	LINK_MESH *faces;
	OBJECT *obj;
	VECTOR ox,oy,oz, axis_pos,axis_size;
	SURF_DATA *cursurf;

	SetVector(&axis_pos, 0.f, 0.f, 0.f);
	SetVector(&ox, 1.f, 0.f, 0.f);
	SetVector(&oy, 0.f, 1.f, 0.f);
	SetVector(&oz, 0.f, 0.f, 1.f);
	SetVector(&axis_size, 1.f, 1.f, 1.f);

	// Allocate Memory for whole chunk (the last word is the surface index for this polygon)
	count = cn->cn_Size/2 - 1;
	polybuffer = ALLOCMEM(WORD, count);
	if (!polybuffer)
		return errors[ERR_MEM];

	// Read polygons
	if(!ReadWord(data->iff, polybuffer, count))
	{
		FREEMEM(polybuffer);
		return errors[ERR_LWOBFILE];
	}

	// go through buffer and look how many edges and faces we need
	i = 0;
	while(i < count)
	{
		cursurf = &data->surfaces[abs(polybuffer[i+polybuffer[i]+1])-1];
		switch(polybuffer[i])
		{
			case 1:
			case 2:
				break;
			case 3:  // triangle
				cursurf->facecount++;
				cursurf->edgecount += 3;
				break;
			case 4:  //rectangle
				cursurf->facecount += 2;
				cursurf->edgecount += 5;
				break;
			default: // polygon
				cursurf->facecount += polybuffer[i] - 2;
				cursurf->edgecount += (polybuffer[i] - 2) * 2 + 1;
				break;
		}
		p = &polybuffer[i+1];
		for(j=0; j<polybuffer[i]; j++)
		{
			if(cursurf->nobound)
			{
				cursurf->lowbound = cursurf->highbound = data->points[*p];
				cursurf->nobound = FALSE;
			}
			else
			{
				if(data->points[*p].x < cursurf->lowbound.x)
					cursurf->lowbound.x = data->points[*p].x;
				if(data->points[*p].y < cursurf->lowbound.y)
					cursurf->lowbound.y = data->points[*p].y;
				if(data->points[*p].z < cursurf->lowbound.z)
					cursurf->lowbound.z = data->points[*p].z;

				if(data->points[*p].x > cursurf->highbound.x)
					cursurf->highbound.x = data->points[*p].x;
				if(data->points[*p].y > cursurf->highbound.y)
					cursurf->highbound.y = data->points[*p].y;
				if(data->points[*p].z > cursurf->highbound.z)
					cursurf->highbound.z = data->points[*p].z;
			}
			p++;
		}
		i += polybuffer[i] + 1;
		// if surface number < 0 then get detail polygons
		if (polybuffer[i] < 0)
		{
			// jump over amount of detail polygons
			i++;
		}
		i++;
	}

	// for each surface go through the polygons and create a single object
	for(j=0; j<data->surfcount; j++)
	{
		data->link->ObjectBegin(data->rc);
		cursurf = &data->surfaces[j];
		if(cursurf->edgecount && cursurf->facecount)
		{
			// Allocate memory for edges
			edges = ALLOCMEM(LINK_EDGE, cursurf->edgecount);
			if (!edges)
			{
				FREEMEM(polybuffer);
				FreeBuffers(data);
				return errors[ERR_MEM];
			}
			// Allocate memory for faces
			faces = ALLOCMEM(LINK_MESH, cursurf->facecount);
			if (!faces)
			{
				FREEMEM(edges);
				FREEMEM(polybuffer);
				FreeBuffers(data);
				return errors[ERR_MEM];
			}

			// and again, go through buffer and now create edges, create only
			// one edge between two points
			i = 0;
			k = 0;   // current face
			edgecount = 0;
			while(i < count)
			{
				if(j == (ULONG)abs(polybuffer[i+polybuffer[i]+1])-1)
				{
					switch(polybuffer[i])
					{
						case 1:
						case 2:
							break;
						case 3:  // triangle
							// search for edge, and create one if no found
							FindEdges(&faces[k], &edgecount, polybuffer[i+1], polybuffer[i+2], polybuffer[i+3], edges);
							k++;
							break;
						case 4:  // rectangle
							// split rectangle in two triangles
							// search for edge, and create one if no found
							FindEdges(&faces[k], &edgecount, polybuffer[i+1], polybuffer[i+2], polybuffer[i+3], edges);
							k++;

							FindEdges(&faces[k], &edgecount, polybuffer[i+3], polybuffer[i+4], polybuffer[i+1], edges);
							k++;
							break;
						default: // polygon
							for(l = 0; l < (ULONG)polybuffer[i]-2; l++)
							{
								FindEdges(&faces[k], &edgecount, polybuffer[i+1], polybuffer[i+l+2], polybuffer[i+l+3], edges);
								k++;
							}
							break;
					}
				}

				i += polybuffer[i] + 1;
				// if surface number < 0 then get detail polygons
				if (polybuffer[i] < 0)
				{
					// jump over amount of detail polygons
					i++;
				}
				i++;
			}

			if(data->link->type == LINK_SCENARIO)
			{
				SetVector(&axis_pos,
					(cursurf->lowbound.x + cursurf->highbound.x) * .5f,
					(cursurf->lowbound.y + cursurf->highbound.y) * .5f,
					(cursurf->lowbound.z + cursurf->highbound.z) * .5f);
				SetVector(&axis_size,
					(cursurf->highbound.x - cursurf->lowbound.x) * .5f,
					(cursurf->highbound.y - cursurf->lowbound.y) * .5f,
					(cursurf->highbound.z - cursurf->lowbound.z) * .5f);

				obj = data->link->MeshCreate(data->rc);
				if(!obj)
					return errors[ERR_MEM];

				if((cursurf->object_count & 0xF) == 0)
				{
					OBJECT **objects;

					objects = new OBJECT*[cursurf->object_count+16];
					if(!objects)
						return errors[ERR_MEM];

					if(cursurf->object_count)
						memcpy(objects, cursurf->objects, cursurf->object_count*sizeof(OBJECT*));

					if(cursurf->objects)
						delete cursurf->objects;

					cursurf->objects = objects;
				}

				cursurf->objects[cursurf->object_count] = obj;
				cursurf->object_count++;

				data->link->ObjectAxis(data->rc, obj, &axis_pos, &ox,&oy,&oz, &axis_size);
				if(cursurf->name)
					data->link->ObjectName(data->rc, obj, cursurf->name);

				if(!data->link->MeshAddScenario(data->rc, obj,
					data->points, data->pointcount,
					edges, cursurf->edgecount,
					faces, cursurf->facecount,
					&data->size,
					FALSE))
				{
					FREEMEM(edges);
					FREEMEM(faces);
					FREEMEM(polybuffer);
					return errors[ERR_MEM];
				}
				data->link->ObjectSurface(data->rc, obj, cursurf->surface);
			}
			else
			{
				if(!data->link->MeshAddRenderer(data->rc, cursurf->surface, NULL, actor,
					data->points, data->pointcount,
					edges, cursurf->edgecount,
					faces, cursurf->facecount,
					&data->pos, &data->ox, &data->oy, &data->oz, &data->size,
					FALSE))
				{
					FREEMEM(edges);
					FREEMEM(faces);
					FREEMEM(polybuffer);
					return errors[ERR_MEM];
				}
			}
			FREEMEM(edges);
			FREEMEM(faces);
		}
		else
		{
			if(data->link->type == LINK_SCENARIO)
			{
				// Scenario needs even empty meshes to create the axis
				obj = data->link->MeshCreate(data->rc);
				if(!obj)
					return errors[ERR_MEM];

				data->link->ObjectAxis(data->rc, obj, &axis_pos, &ox,&oy,&oz, &axis_size);
				if(cursurf->name)
					data->link->ObjectName(data->rc, obj, cursurf->name);
				data->link->ObjectSurface(data->rc, obj, cursurf->surface);
			}
		}
		data->link->ObjectEnd(data->rc);
		if(data->SetProgress)
			data->SetProgress(data->rc, ((float)data->size_done + (float)(cn->cn_Size*j)/(float)data->surfcount)/(float)data->filesize);
	}

	// free polygonbuffer
	if(polybuffer)
		FREEMEM(polybuffer);

	return NULL;
}

/*************
 * DESCRIPTION:   read the surfaces
 * INPUT:         data     handler data
 *                cn       iff context node
 * OUTPUT:        error string
 *************/
static char *ReadSurfaces(HANDLER_DATA *data, struct ContextNode *cn)
{
	char *buffer, *name;
	int current;
	BOOL done;
	int len;

	buffer = ALLOCMEM(char, cn->cn_Size);
	if(!buffer)
		return errors[ERR_MEM];

	if(ReadChunkBytes(data->iff, buffer, cn->cn_Size) != cn->cn_Size)
	{
		FREEMEM(buffer);
		return errors[ERR_LWOBFILE];
	}

	// count names
	name = buffer;
	data->surfcount = 0;
	done = FALSE;
	while(!done)
	{
		data->surfcount++;
		len = strlen(name)+1;
		len += len & 1;
		name = name + len;
		if(name - buffer >= cn->cn_Size)
			done = TRUE;
	}

	data->surfaces = ALLOCMEM(SURF_DATA, data->surfcount);
	if(!data->surfaces)
	{
		FREEMEM(buffer);
		return errors[ERR_MEM];
	}

	name = buffer;
	done = FALSE;
	current = 0;
	while(!done)
	{
		if(!data->replacesurf)
		{
			data->surfaces[current].surface = data->link->SurfaceAdd(data->rc);
			if(!data->surfaces[current].surface)
			{
				FREEMEM(buffer);
				return errors[ERR_LWOBFILE];
			}
		}
		else
			data->surfaces[current].surface = data->replacesurf;
		len = strlen(name)+1;
		data->surfaces[current].name = new char[len];
		if(!data->surfaces[current].name)
		{
			FREEMEM(buffer);
			return errors[ERR_LWOBFILE];
		}
		strcpy(data->surfaces[current].name, name);
		if(!data->replacesurf)
			data->link->SurfaceName(data->rc, data->surfaces[current].surface, name);

		current++;
		len += len & 1;
		name = name + len;
		if(name - buffer >= cn->cn_Size)
			done = TRUE;
	}
	return NULL;
}

/*************
 * DESCRIPTION:   set the texture information
 * INPUT:         data     handler data
 *    surf_name   surface name
 *    brush       pointer to brush
 *    type        texture type
 *    wrap_type   wrap type string
 *    flags       brush flags
 *    x_mode      mode flags in x-direction
 *    y_mode      mode flags in y-direction
 *    pos         position
 *    size        size
 * OUTPUT:        error string
 *************/
static char *SetTexture(HANDLER_DATA *data, char *surf_name, BRUSH *brush, ULONG type, char *wrap_type, ULONG flags,
	UWORD x_mode, UWORD y_mode, VECTOR *brush_pos, VECTOR *brush_size)
{
	BRUSH_OBJECT *brush_object;
	int i;
	SURF_DATA *surf_data;
	ULONG brush_flags;
	VECTOR ox,oy,oz;
	VECTOR align = {0.f, 0.f, 0.f};

	brush_flags = 0;
	if(flags & FLAGS_TEXTURE_BLEND)
		brush_flags |= LINK_BRUSH_SOFT;

	switch(type)
	{
		case TEXTURE_DIFFUSE:
			brush_flags |= LINK_BRUSH_MAP_COLOR;
			break;
		case TEXTURE_SPECULAR:
			brush_flags |= LINK_BRUSH_MAP_SPECULAR;
			break;
		case TEXTURE_REFLECTION:
			brush_flags |= LINK_BRUSH_MAP_REFLECTIFITY;
			break;
		case TEXTURE_TRANSPARENCY:
			brush_flags |= LINK_BRUSH_MAP_FILTER;
			break;
		case TEXTURE_BUMP:
			brush_flags |= LINK_BRUSH_MAP_ALTITUDE;
			break;
	}

	// RayStorm don't supports independant repeat flags so repeat
	// if one of the flags is set
	if((x_mode == TEXTURE_REPEAT) || (y_mode == TEXTURE_REPEAT))
	{
		brush_flags |= LINK_BRUSH_REPEAT;
	}

	if(wrap_type)
	{
		if(!strcmp(wrap_type, "Planar Image Map"))
		{
			brush_flags |= 0;
			if(flags & FLAGS_TEXTURE_XAXIS)
				align.y = 90.f;
			if(flags & FLAGS_TEXTURE_YAXIS)
				align.x = 90.f;
		}
		else if(!strcmp(wrap_type, "Cylindrical Image Map"))
		{
			if(flags & FLAGS_TEXTURE_XAXIS)
			{
				brush_flags |= LINK_BRUSH_WRAPX;
			}
			if(flags & FLAGS_TEXTURE_YAXIS)
			{
				brush_flags |= LINK_BRUSH_WRAPY;
			}
			if(flags & FLAGS_TEXTURE_ZAXIS)
			{
				align.x = 90.f;
				brush_flags |= LINK_BRUSH_WRAPY;
			}
		}
		else if(!strcmp(wrap_type, "Spherical Image Map"))
		{
			brush_flags |= LINK_BRUSH_WRAPX | LINK_BRUSH_WRAPY;
			if(flags & FLAGS_TEXTURE_XAXIS)
				align.z = 90.f;
			if(flags & FLAGS_TEXTURE_ZAXIS)
				align.x = 90.f;
		}
		else if(!strcmp(wrap_type, "Cubic Image Map"))
			brush_flags |= 0;
		else if(!strcmp(wrap_type, "Front Projection Image Map"))
			brush_flags |= 0;
	}

	data->link->BrushFlags(data->rc, brush, brush_flags);

	i = 0;
	surf_data = NULL;
	while(!surf_data && i<data->surfcount)
	{
		if(!strcmp(surf_name, data->surfaces[i].name))
			surf_data = &data->surfaces[i];
		i++;
	}

	if(surf_data)
	{
		CalcOrient(&align, &ox,&oy,&oz);

		brush_object = NULL;
		for(i=0; i<surf_data->object_count; i++)
		{
			brush_object = data->link->BrushObjectAdd(data->rc, surf_data->objects[i], NULL, brush_object);
			if(!brush_object)
				return errors[ERR_MEM];
			data->link->BrushPos(data->rc, brush_object, brush_pos);
			data->link->BrushSize(data->rc, brush_object, brush_size);
			data->link->BrushOrient(data->rc, brush_object, &ox,&oy,&oz);
			if(data->link->type == LINK_RENDERER)
				data->link->BrushAlignment(data->rc, brush_object, &align);
		}
	}

	return NULL;
}

/*************
 * DESCRIPTION:   read the surface information
 * INPUT:         data     handler data
 *                cn       iff context node
 * OUTPUT:        error string
 *************/
static char *ReadSurface(HANDLER_DATA *data, struct ContextNode *cn)
{
	UBYTE *buffer;
	ULONG cur = 0, size;
	ULONG id;
	float value;
	SURFACE *surf;
	float r,g,b;
	float spec=0.f, refl=0.f, tran=0.f;
	UWORD flags = 0, brush_flags = 0;
	UWORD gloss, x_mode, y_mode;
	char *surf_name, *wrap_type;
	char *err;
	ULONG type = TEXTURE_NONE;
	BRUSH *brush = NULL;
	VECTOR brush_pos = {0.f,0.f,0.f},brush_size = {1.f,1.f,1.f};

	buffer = ALLOCMEM(UBYTE, cn->cn_Size);
	if(!buffer)
		return errors[ERR_MEM];

	if(ReadChunkBytes(data->iff, buffer, cn->cn_Size) != cn->cn_Size)
	{
		FREEMEM(buffer);
		return errors[ERR_LWOBFILE];
	}

	surf_name = (char*)buffer;
	surf = data->link->SurfaceGetByName(data->rc, (char*)buffer);
	if(!surf)
	{
		// If we can't find the surface it isn't used as a main surface
		// from a polygon. So we can ignore this surface.
		FREEMEM(buffer);
		return NULL;
	}

	cur += strlen((char*)buffer)+1;
	if(cur & 1)
		cur++;

	while(cur < (cn->cn_Size-4))
	{
		id = READULONG(cur);
		cur += 4;
		size = READUWORD(cur);
		cur += 2;
		switch(id)
		{
			case ID_COLR:
				READ_COLOR(buffer, r,g,b);
				data->link->SurfaceDiffuse(data->rc, surf, r,g,b);
				break;
			case ID_SPEC:
				spec = ((float)READUWORD(cur)) / 256.f;
				break;
			case ID_REFL:
				refl = ((float)READUWORD(cur)) / 256.f;
				break;
			case ID_TRAN:
				tran = ((float)READUWORD(cur)) / 256.f;
				break;
			case ID_FLAG:
				flags = READUWORD(cur);
				break;
			case ID_RIND:
				value = *(float*)(&buffer[cur]);
				data->link->SurfaceIndOfRefr(data->rc, surf, value);
				break;
			case ID_GLOS:
				gloss = READUWORD(cur);
				data->link->SurfaceRefPhong(data->rc, surf, (float)gloss);
				break;
			case ID_CTEX:
			case ID_DTEX:
				if((type != TEXTURE_NONE) && brush)
				{
					err = SetTexture(data, surf_name, brush, type, wrap_type, brush_flags, x_mode, y_mode, &brush_pos, &brush_size);
					if(err)
					{
						FREEMEM(buffer);
						return err;
					}
				}
				type = TEXTURE_DIFFUSE;
				wrap_type = (char*)&buffer[cur];
				break;
			case ID_STEX:
				if((type != TEXTURE_NONE) && brush)
				{
					err = SetTexture(data, surf_name, brush, type, wrap_type, brush_flags, x_mode, y_mode, &brush_pos, &brush_size);
					if(err)
					{
						FREEMEM(buffer);
						return err;
					}
				}
				type = TEXTURE_SPECULAR;
				wrap_type = (char*)&buffer[cur];
				break;
			case ID_RTEX:
				if((type != TEXTURE_NONE) && brush)
				{
					err = SetTexture(data, surf_name, brush, type, wrap_type, brush_flags, x_mode, y_mode, &brush_pos, &brush_size);
					if(err)
					{
						FREEMEM(buffer);
						return err;
					}
				}
				type = TEXTURE_REFLECTION;
				wrap_type = (char*)&buffer[cur];
				break;
			case ID_TTEX:
				if((type != TEXTURE_NONE) && brush)
				{
					err = SetTexture(data, surf_name, brush, type, wrap_type, brush_flags, x_mode, y_mode, &brush_pos, &brush_size);
					if(err)
					{
						FREEMEM(buffer);
						return err;
					}
				}
				type = TEXTURE_TRANSPARENCY;
				wrap_type = (char*)&buffer[cur];
				break;
			case ID_BTEX:
				if((type != TEXTURE_NONE) && brush)
				{
					err = SetTexture(data, surf_name, brush, type, wrap_type, brush_flags, x_mode, y_mode, &brush_pos, &brush_size);
					if(err)
					{
						FREEMEM(buffer);
						return err;
					}
				}
				type = TEXTURE_BUMP;
				wrap_type = (char*)&buffer[cur];
				break;
			case ID_TIMG:
				brush = data->link->BrushAdd(data->rc, (char*)&buffer[cur], surf);
				if(!brush)
				{
					FREEMEM(buffer);
					return errors[ERR_MEM];
				}
				break;
			case ID_TFLG:
				brush_flags = READUWORD(cur);
				break;
			case ID_TWRP:
				x_mode = READUWORD(cur);
				y_mode = READUWORD(cur+2);
				break;
			case ID_TSIZ:
				brush_size.x = *(float*)(&buffer[cur]);
				brush_size.y = *(float*)(&buffer[cur+4]);
				brush_size.z = *(float*)(&buffer[cur+8]);
				break;
			case ID_TCTR:
				brush_pos.x = *(float*)(&buffer[cur]);
				brush_pos.y = *(float*)(&buffer[cur+4]);
				brush_pos.z = *(float*)(&buffer[cur+8]);
				break;
		}
		cur += size;
	}

	if((type != TEXTURE_NONE) && brush)
	{
		err = SetTexture(data, surf_name, brush, type, wrap_type, brush_flags, x_mode, y_mode, &brush_pos, &brush_size);
		if(err)
		{
			FREEMEM(buffer);
			return err;
		}
	}


	if(flags & FLAGS_COLOR_HIGHLIGHTS)
		data->link->SurfaceSpecular(data->rc, surf, r*spec, g*spec, b*spec);
	else
		data->link->SurfaceSpecular(data->rc, surf, spec, spec, spec);
	data->link->SurfaceReflect(data->rc, surf, refl, refl, refl);
	if(flags & FLAGS_COLOR_FILTER)
		data->link->SurfaceTranspar(data->rc, surf, r*tran, g*tran, b*tran);
	else
		data->link->SurfaceTranspar(data->rc, surf, tran, tran, tran);

	FREEMEM(buffer);
	return NULL;
}

/*************
 * DESCRIPTION:   read a LWOB-file
 * INPUT:         rc          context
 *                filename    name of LWOB-file
 *                link        link structure
 *                pos         object position
 *                ox, oy, oz  object orientation
 *                actor       pointer to actor
 *                replacesurf pointer to surface to replace object surface
 *                            with
 *                version     version number
 * OUTPUT:        NULL if ok else errorstring
 *************/
extern "C" char* SAVEDS objRead(rsiCONTEXT *rc, char *filename, OBJLINK *link, VECTOR *pos, VECTOR *ox, VECTOR *oy,
	VECTOR *oz, VECTOR *scale, ACTOR *actor, SURFACE *replacesurf, ULONG *version, void (*SetProgress)(rsiCONTEXT*, float))
{
	long error = 0;
	struct ContextNode *cn;
	char *err;
	HANDLER_DATA data;

	data.link = link;
	data.iff = NULL;
	data.points = NULL;
	data.rc = rc;
	data.surfaces = NULL;
	data.pos = *pos;
	data.ox = *ox;
	data.oy = *oy;
	data.oz = *oz;
	data.size = *scale;
	data.replacesurf = replacesurf;
	data.SetProgress = SetProgress;
	data.size_done = 0;

	err = OpenIFF(&data.iff, filename, &data.filesize);
	if(err)
	{
		readLWOB_cleanup(&data);
		return err;
	}

	while (!error || error == IFFERR_EOC)
	{
		error = ParseIFF(data.iff, IFFPARSE_RAWSTEP);
		if (error != IFFERR_EOC)
		{
			// Get a pointer to the context node describing the current context
			cn = CurrentChunk(data.iff);
			if (cn)
			{
				switch (cn->cn_ID)
				{
					case ID_SRFS:
						err = ReadSurfaces(&data, cn);
						if(err)
						{
							readLWOB_cleanup(&data);
							return err;
						}
						break;
					case ID_SURF:
						if(!data.replacesurf)
						{
							err = ReadSurface(&data, cn);
							if(err)
							{
								readLWOB_cleanup(&data);
								return err;
							}
						}
						break;
					case ID_PNTS:
						err = ReadPoints(&data, cn);
						if(err)
						{
							readLWOB_cleanup(&data);
							return err;
						}
						break;
					case ID_POLS:
						err = ReadPolygons(&data, cn, actor);
						if(err)
						{
							readLWOB_cleanup(&data);
							return err;
						}
						break;
				}
				if(SetProgress)
				{
					data.size_done += cn->cn_Size;
					(*SetProgress)(rc, (float)data.size_done/(float)data.filesize);
				}
			}
		}
	}
	readLWOB_cleanup(&data);
	if (error != IFFERR_EOF)
	{
		return errors[ERR_LWOBFILE];
	}
	return NULL;
}

/*************
 * DESCRIPTION:   free mem and iff-handle
 * INPUT:         data     handler data
 * OUTPUT:        none
 *************/
static void readLWOB_cleanup(HANDLER_DATA *data)
{
	FreeBuffers(data);

	if(data->iff)
	{
		CloseIFF(data->iff);
		if(data->iff->iff_Stream)
			Close(data->iff->iff_Stream);
		FreeIFF(data->iff);
		data->iff = NULL;
	}
}

/*************
 * DESCRIPTION:   Cleanup TDDD handler
 * INPUT:         none
 * OUTPUT:        none
 *************/
extern "C" void SAVEDS objCleanup()
{
}

