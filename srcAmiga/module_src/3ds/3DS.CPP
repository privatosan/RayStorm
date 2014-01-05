/*************
 * MODULE:      3DS-object format handler v1.04
 * NAME:        3ds.cpp
 * PROJECT:     RayStorm
 * DESCRIPTION: This module keeps functions to read 3ds-files
 * AUTHORS:     Mike Hesser, Andreas Heumann
 * TO DO:       - names, hierarchy
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    --------------------------------------------------------------------------
 *    21.11.95  mh      initial version
 *    22.11.95  mh      main read loop, ReadASCIIZ
 *    24.11.95  mh      Open3DS, Close3DS, ReadChunkBytes, ReadWord, ReadFloat
 *    25.11.95  mh      compilable version
 *    26.11.95  mh      can load data, but the positions of the triangels are totally wrong
 *    16.12.95  mh      loading of ambient, diffuse, ...
 *    18.12.95  mh      everything works, except some attributes and phong interpolation
 *    11.01.96  mh      smooth triangles
 *    12.01.96  mh      error handling
 *    17.01.96  mh      first release
 *    14.04.96  ah      generate triangle normals for renderer only
 *    18.04.96  ah      added replacesurf to Read3DS
 *    04.10.96  mh      bugfix: defaultsurface had no name -> crash with CreateUniqueName
 *    06.10.96  mh      speedup of VecAngle, cos_smooth_angle
 *    17.01.96  mh      bugfix: INCHUNK in while statement caused endless loop in pathological cases
 *    15.05.97  ah      added version
 *    25.06.97  ah      changed to new V2 style
 *    10.05.99  ah      added parsing of texture mapping coordinates
 *    10.05.99  ah      using a replay surface crashed -> fixed
 *************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef __AMIGA__
#include <pragma/iffparse_lib.h>
#include <pragma/dos_lib.h>
#include "rmod:objlib.h"
#pragma libbase ObjHandBase
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

class SURFACE;
class TRIANGLE;
class OBJECT;
class BRUSH;
class BRUSH_OBJECT;
class TEXTURE;
class TEXTURE_OBJECT;
class PLANE;
class SPHERE;
class BOX;
class CYLINDER;
class CONE;
class ACTOR;
class CAMERA;
class LIGHT;
class POINT_LIGHT;
class SPOT_LIGHT;
class FLARE;
class STAR;
typedef void rsiCONTEXT;

#ifndef OBJLINK_H
	#include "objlink.h"
#endif

#ifndef DDDS_H
	#include "3ds.h"
#endif

enum errnums
{
	OK, ERR_MEM, ERR_OPEN, ERR_READ, ERR_NO3DS
};

static char *errors[] =
{
	NULL,
	"Not enough memory",
	"Can't open 3DS file",
	"Error reading 3DS file",
	"No 3DS file"
};

typedef struct TRILIST_
{
	TRILIST_    *next;
	UWORD    tri;
	UBYTE    flag;
} TRILIST;

#define INCHUNK (chunk.end < mainchunk->end && !data->err)
#define INCHUNK2 (chunk.end <= mainchunk->end && !data->err)

// local variables
typedef struct
{
	OBJLINK    *link;
	DDDSHandle *hFile;
	VECTOR     *points;
	VECTOR     *VertNorms;
	FACE3DS    *face;
	SURFACE    **material;
	VECT2D     *mapping;
	float      transm[12];
	char       ObjName[80];
	MATRIX     matrix, alignmatrix, transmatrix;
	SURFACE    *defaultsurface, *replacesurface;
	UWORD      pointcount, facecount, mapcount;
	ACTOR      *mainactor;
	float      smooth_angle, cos_smooth_angle;
	VECTOR     *TriNorms;
	UBYTE      *TriSmooth;
	TRILIST    **TriList;
	int        err;
	rsiCONTEXT *rc;
} HANDLER_DATA;

// prototypes
static BOOL VecZero(const VECTOR);
static float VecAngle(const VECTOR, const VECTOR);
static void SetSmoothAngle(HANDLER_DATA*, float);
static DDDSHandle *Open3DS(const char *);
static void Close3DS(HANDLER_DATA*);
static BOOL ReadWord(HANDLER_DATA*, WORD *, int);
static BOOL ReadFloat(HANDLER_DATA*, float *, int);
static void ReadASCIIZ(HANDLER_DATA*, char *);
static void BeginChunk(HANDLER_DATA*, CHUNK *);
static void EndChunk(HANDLER_DATA*, CHUNK *);
static void ParseColor(HANDLER_DATA*, COLOR *);
static float ParsePercentage(HANDLER_DATA*);
static void ParseFaces(HANDLER_DATA*, CHUNK *);
static void ParsePoints(HANDLER_DATA*);
static void ParseTriObject(HANDLER_DATA*, CHUNK *);
static void ParsePropMat(HANDLER_DATA*, CHUNK *);
static void ParseNamedObject(HANDLER_DATA*, CHUNK *);
static void ParseMData(HANDLER_DATA*, CHUNK *);
static void CleanupMesh(HANDLER_DATA*);

extern "C" void Cleanup3DS(HANDLER_DATA*);

#undef SAVEDS
#ifdef __PPC__
#define SAVEDS __saveds
#else
#define SAVEDS
#endif

/*************
 * DESCRIPTION:   Init object handler, fill in the needed functions, open libraries
 * INPUT:         none
 * OUTPUT:        FALSE if failed
 *************/
extern "C" BOOL SAVEDS objInit()
{
	return TRUE;
}

/*************
 * DESCRIPTION:   test is zero vector
 * INPUT:         v   Vector
 * OUTPUT:        TRUE if vector zero
 *************/
static BOOL VecZero(const VECTOR v)
{
	return !(v.x != 0.0f || v.y != 0.0f || v.z != 0.0f);
}

/*************
 * DESCRIPTION:   calculates angle between two vectors
 * INPUT:         v1, v2    Vectors (unit length!)
 * OUTPUT:        angle between two vectors
 *************/
static float VecAngle(const VECTOR v1, const VECTOR v2)
{
	float cos_theta;

	cos_theta = dotp(&v1, &v2);

	if (cos_theta < -1.0f)
		cos_theta = PI;
	else
		if (cos_theta > 1.0f)
			cos_theta = 0.f;
		else
			return acos(cos_theta);
/* if (cos_theta < -1.0f)
		cos_theta = -1.0f;
	else
		if (cos_theta > 1.0f)
			cos_theta = 1.0f;
*/
	return cos_theta;
}

/*************
 * DESCRIPTION:   sets the smooth angle
 * INPUT:         smooth    degree
 * OUTPUT:        -
 *************/
void SetSmoothAngle(HANDLER_DATA *data, float smooth)
{
	if (smooth >= 0.0f && smooth <= 180.0f)
		data->smooth_angle = smooth*PI/180.f;
	else
		data->smooth_angle = 70*PI/180.f;

	data->cos_smooth_angle = cos(data->smooth_angle*PI/180.f);
}
// } <- needed by GoldED

/*************
 * DESCRIPTION:   open a 3DS file
 * INPUT:       filename   name of file
 * OUTPUT:      NULL when failed else filehandle
 *************/
static DDDSHandle *Open3DS(const char *filename)
{
	return(fopen(filename, "rb"));
}

/*************
 * DESCRIPTION:   close 3DS file
 * INPUT:       -
 * OUTPUT:      -
 *************/
static void Close3DS(HANDLER_DATA *data)
{
	fclose(data->hFile);
}

/*************
 * DESCRIPTION:   Read size bytes from file
 * INPUT:       buf pointer to buffer
 *              size size of block to be loaded
 * OUTPUT:      number of bytes read
 *************/
static long ReadChunkBytes(HANDLER_DATA *data, void *buf, long size)
{
	return fread(buf, 1, size, data->hFile);
}

/*************
 * DESCRIPTION:   Read word from file
 * INPUT:       buf   pointer to buffer
 *              cnt   number of words
 * OUTPUT:      FALSE if failed, else TRUE
 *************/
static BOOL ReadWord(HANDLER_DATA *data, WORD *buf, int cnt)
{
#ifndef __SWAP__
	BOOL res;
	int  i;

	res = (ReadChunkBytes(data, buf, sizeof(WORD) * cnt) == sizeof(WORD) * cnt);
	for(i = 0; i < cnt; i++)
		buf[i] = SwapW(buf[i]);
	return res;
#else
	return (ReadChunkBytes(data, buf, sizeof(WORD) * cnt) == sizeof(WORD) * cnt);
#endif
}

/*************
 * DESCRIPTION:   Read floats from file
 * INPUT:       buf    pointer to buffer
 *              cnt    number of fracts
 * OUTPUT:      FALSE if failed, else TRUE
 *************/
static BOOL ReadFloat(HANDLER_DATA *data, float *buf, int cnt)
{
#ifndef __SWAP__
	BOOL res;
	int  i;
	ULONG *b;

	b = (ULONG*)buf;
	res = (ReadChunkBytes(data, b, sizeof(float) * cnt) == sizeof(float) * cnt);
	for (i = 0; i < cnt; i++)
		b[i] = Swap(b[i]);
	buf = (float*)b;
	return res;
#else
	return (ReadChunkBytes(data, buf, sizeof(float) * cnt) == sizeof(float) * cnt);
#endif
}

/*************
 * DESCRIPTION: reads a zero terminated string
 * INPUT:       buf    pointer to textbuffer
 * OUTPUT:      -
 *************/
static void ReadASCIIZ(HANDLER_DATA *data, char *buf)
{
	int i = 0;

	// read until zero byte
	do
	{
		ReadChunkBytes(data, &buf[i], 1);
	}
	while (buf[i++]);
}

/*************
 * DESCRIPTION: reads id and position of next chunk
 * INPUT:       chunk     pointer to chunk structure
 * OUTPUT:      -
 *************/
static void BeginChunk(HANDLER_DATA *data, CHUNK *chunk)
{
	ULONG next;

	// get chunk id
	ReadWord(data, (WORD*)&chunk->id, 1);
	// and position of next chunk
	ReadChunkBytes(data, &next, 4);
#ifndef __SWAP__
	next = Swap(next);
#endif
	chunk->end = ftell(data->hFile) + next - 6;
}

/*************
 * DESCRIPTION: jumps to next chunk
 * INPUT:       chunk     pointer to chunk structure
 * OUTPUT:      -
 *************/
static void EndChunk(HANDLER_DATA *data, CHUNK *chunk)
{
	fseek(data->hFile, chunk->end, SEEK_SET);
}

/*************
 * DESCRIPTION: read a color
 * INPUT:       color   pointer to color
 * OUTPUT:      -
 *************/
static void ParseColor(HANDLER_DATA *data, COLOR *color)
{
	CHUNK chunk;
	UBYTE c[3];

	BeginChunk(data, &chunk);

	switch (chunk.id)
	{
		case ID_COLOR1:
				ReadFloat(data, &color->r, 1);
				ReadFloat(data, &color->g, 1);
				ReadFloat(data, &color->b, 1);
				break;

		case ID_COLOR255:
				ReadChunkBytes(data, &c, 3);
				color->r = c[0]/255.0f;
				color->g = c[1]/255.0f;
				color->b = c[2]/255.0f;
				break;
	}
	EndChunk(data, &chunk);
}

/*************
 * DESCRIPTION: read a percent value
 * INPUT:       -
 * OUTPUT:      float (0..1)
 *************/
static float ParsePercentage(HANDLER_DATA *data)
{
	CHUNK chunk;
	float percent = 0.f;

	BeginChunk(data, &chunk);

	switch (chunk.id)
	{
		case ID_PERCENT100:
				ReadFloat(data, &percent, 1);
				percent*= 0.01f;
				break;
		case ID_PERCENT1:
				ReadChunkBytes(data, &percent, 1);
				break;
	}

	EndChunk(data, &chunk);

	return percent;
}

/*************
 * DESCRIPTION: read faces of object
 * INPUT:       pointer to chunk
 * OUTPUT:      -
 *************/
static void ParseFaces(HANDLER_DATA *data, CHUNK *mainchunk)
{
	CHUNK    chunk;
	UWORD    i, matcount, index, p1, p2, p3;
	VECTOR   e1, e2;
	SURFACE  *s;
	TRILIST  *hp;
	char     buf[80];

	ReadWord(data, (WORD *)&data->facecount, 1); // read number of faces

	if (data->facecount == 0)
		return;

	data->face = new FACE3DS[data->facecount];
	if (!data->face)
	{
		data->err = ERR_MEM;
		return;
	}
	if (!data->replacesurface)
	{
		data->material = (SURFACE **)malloc(sizeof(SURFACE *)*data->facecount);
		if (!data->material)
		{
			data->err = ERR_MEM;
			return;
		}
	}

	ReadWord(data, (WORD *)data->face, 4*data->facecount);   // read faces

	if (data->link->type == LINK_RENDERER)
	{
		// do it for renderer only

		data->VertNorms = new VECTOR[data->pointcount];
		if (!data->VertNorms)
		{
			data->err = ERR_MEM;
			return;
		}

		data->TriNorms = new VECTOR[data->facecount];
		if (!data->TriNorms)
		{
			data->err = ERR_MEM;
			return;
		}
		memset(data->VertNorms, 0, sizeof(VECTOR)*data->pointcount);   // Init normals

		data->TriSmooth = new UBYTE[data->facecount];
		if (!data->TriSmooth)
		{
			data->err = ERR_MEM;
			return;
		}

		for (i = 0; i < data->facecount; i++)
		{
			if(!data->replacesurface)
				data->material[i] = data->replacesurface;

			data->TriSmooth[i] = FALSE;
			// get three points for the triangle
			p1 = data->face[i].p1;
			p2 = data->face[i].p3;
			p3 = data->face[i].p2;

			hp = new TRILIST;
			if (!hp)
			{
				data->err = ERR_MEM;
				return;
			}
			hp->next = data->TriList[p1];
			hp->tri = i;
			hp->flag = FALSE;
			data->TriList[p1] = hp;

			hp = new TRILIST;
			if (!hp)
			{
				data->err = ERR_MEM;
				return;
			}
			hp->next = data->TriList[p2];
			hp->tri = i;
			hp->flag = FALSE;
			data->TriList[p2] = hp;

			hp = new TRILIST;
			if (!hp)
			{
				data->err = ERR_MEM;
				return;
			}
			hp->next = data->TriList[p3];
			hp->tri = i;
			hp->flag = FALSE;
			data->TriList[p3] = hp;

			// calculate normal of triangle
			VecSub(&data->points[p3], &data->points[p1], &e1);
			VecSub(&data->points[p2], &data->points[p1], &e2);
			VecNormCross(&e1, &e2, &data->TriNorms[i]);
		}
	}
	do
	{
		BeginChunk(data, &chunk);
		switch (chunk.id)
		{
			case ID_MSHMATGROUP:
					if (!data->replacesurface)
					{
						ReadASCIIZ(data, buf);
						s = data->link->SurfaceGetByName(data->rc, buf);
						ReadWord(data, (WORD*)&matcount, 1);
						for (i = 0; i < matcount; i++)
						{
							ReadWord(data, (WORD*)&index, 1);
							data->material[index] = s;
						}
					}
					break;
			case ID_SMOOTHGROUP: // no info about this group
					break;
		}
		EndChunk(data, &chunk);
	}
	while (INCHUNK);
}

/*************
 * DESCRIPTION: read faces of object
 * INPUT:       -
 * OUTPUT:      -
 *************/
static void ParsePoints(HANDLER_DATA *data)
{
	VECTOR  *p;
	int i;

	// read number of points
	ReadWord(data, (WORD *)&data->pointcount, 1);

	if (data->pointcount == 0)
		return;

	data->points = new VECTOR[data->pointcount]; // Allocate memory
	if (!data->points)
	{
		data->err = ERR_MEM;
		return;
	}
	ReadFloat(data, (float *)data->points, data->pointcount*3); // Read points

	data->TriList = (TRILIST**)malloc(sizeof(TRILIST*)*data->pointcount);
	if (!data->TriList)
	{
		data->err = ERR_MEM;
		return;
	}

	// apply transformations to the points
	p = data->points;
	for (i = 0; i < data->pointcount; i++)
	{
		data->TriList[i] = NULL;
		data->matrix.MultVectMat(p);
		p++;
	}
}


/*************
 * DESCRIPTION: read mapping coordinates of object
 * INPUT:       -
 * OUTPUT:      -
 *************/
static void ParseMapping(HANDLER_DATA *data)
{
	// read number of mapping coordinates
	ReadWord(data, (WORD *)&data->mapcount, 1);

	if (data->mapcount == 0)
		return;

	data->mapping = new VECT2D[data->mapcount]; // Allocate memory
	if (!data->mapping)
	{
		data->err = ERR_MEM;
		return;
	}
	ReadFloat(data, (float *)data->mapping, data->mapcount*2); // Read mapping coordinates
}

/*************
 * DESCRIPTION: read an object consisting of
 *              triangles
 * INPUT:       pointer to chunk
 * OUTPUT:      -
 *************/
static void ParseTriObject(HANDLER_DATA *data, CHUNK *mainchunk)
{
	CHUNK chunk;

	do
	{
		BeginChunk(data, &chunk);
		switch (chunk.id)
		{
			case ID_POINTS:
				ParsePoints(data);
				break;
			case ID_FACES:
				ParseFaces(data, &chunk);
				break;
			case ID_MAPPINGCOORS:
				ParseMapping(data);
				break;
			case ID_TRANSMATRIX:
				ReadFloat(data, &data->transm[0], 12);
				data->transmatrix.m[0] = data->transm[0];
				data->transmatrix.m[1] = data->transm[1];
				data->transmatrix.m[2] = data->transm[2];
				data->transmatrix.m[4] = data->transm[3];
				data->transmatrix.m[5] = data->transm[4];
				data->transmatrix.m[6] = data->transm[5];
				data->transmatrix.m[8] = data->transm[6];
				data->transmatrix.m[9] = data->transm[7];
				data->transmatrix.m[10] = data->transm[8];
				data->transmatrix.m[12] = data->transm[9];
				data->transmatrix.m[13] = data->transm[10];
				data->transmatrix.m[14] = data->transm[11];
				break;
		}
		EndChunk(data, &chunk);
	}
	while (INCHUNK);
}

/*************
 * DESCRIPTION: read a material-chunk
 * INPUT:       pointer to chunk
 * OUTPUT:      -
 *************/
static void ParsePropMat(HANDLER_DATA *data, CHUNK *mainchunk)
{
	CHUNK    chunk;
	SURFACE  *surf;
	float    perc;
	COLOR    color, diffuse;
	char     buf[80];

	surf = data->link->SurfaceAdd(data->rc);
	if (!surf)
	{
		data->err = ERR_MEM;
		return;
	}
	do
	{
		BeginChunk(data, &chunk);
		switch (chunk.id)
		{
			case ID_PROPNAME:
					ReadASCIIZ(data, buf);
					if (!data->link->SurfaceName(data->rc, surf,buf))
					{
						data->err = ERR_MEM;
						return;
					}
					break;
			case ID_AMBIENT:
					ParseColor(data, &color);
					data->link->SurfaceAmbient(data->rc, surf,color.r,color.g,color.b);
					break;
			case ID_DIFFUSE:
					ParseColor(data, &diffuse);
					data->link->SurfaceDiffuse(data->rc, surf,diffuse.r,diffuse.g,diffuse.b);
					break;
			case ID_SPECULAR:
					ParseColor(data, &color);
					data->link->SurfaceSpecular(data->rc, surf,color.r,color.g,color.b);
					break;
			case ID_SHININESS:
					perc = ParsePercentage(data);
					data->link->SurfaceRefPhong(data->rc, surf,perc*100.f);
					break;
			case ID_TRANSPARENCY:
					perc = ParsePercentage(data);
					data->link->SurfaceTranspar(data->rc, surf,perc*diffuse.r,perc*diffuse.g,perc*diffuse.b);
					break;
		}
		EndChunk(data, &chunk);
	}
	while (INCHUNK);
}

/*************
 * DESCRIPTION: -
 * INPUT:       pointer to chunk
 * OUTPUT:      -
 *************/
static void ParseNamedObject(HANDLER_DATA *data, CHUNK *mainchunk)
{
	CHUNK     chunk;
	TRIANGLE *triangle;
	TRILIST  *ph1,*ph2;
	float     angle;
	UWORD     p1, p2, p3;
	UWORD     *edges;
	int       i, h;

	ReadASCIIZ(data, data->ObjName);
	do
	{
		BeginChunk(data, &chunk);
		switch (chunk.id)
		{
			case ID_TRIANGLE:
					ParseTriObject(data, &chunk);
					break;
		}
		EndChunk(data, &chunk);
	}
	while (INCHUNK);

	if (data->TriList && (data->link->type == LINK_RENDERER))
	{
		// go through all vertices and calculate normals (only for renderer)
		for (i = 0; i < data->pointcount; i++)
		{
			data->VertNorms[i].x = data->VertNorms[i].y = data->VertNorms[i].z = 0;
			ph1 = data->TriList[i];
			while (ph1)
			{
				for (ph2 = ph1->next; ph2 != NULL; ph2 = ph2->next)
				{
					if (!ph1->flag || !ph2->flag)
					{
						// test angle between two triangles
						angle = VecAngle(data->TriNorms[ph1->tri], data->TriNorms[ph2->tri]);
//                if (angle < 2*PI && angle > /*cos_*/smooth_angle)
						if (angle >0 && angle < /*cos_*/data->smooth_angle)
						{
							if (!ph1->flag)
							{
								VecAdd(&data->VertNorms[i], &data->TriNorms[ph1->tri], &data->VertNorms[i]);
								ph1->flag = TRUE;
								data->TriSmooth[ph1->tri] = TRUE;
							}
							if (!ph2->flag)
							{
								VecAdd(&data->VertNorms[i], &data->TriNorms[ph2->tri], &data->VertNorms[i]);
								ph2->flag = TRUE;
								data->TriSmooth[ph2->tri] = TRUE;
							}
						}
					}
				}
				ph2 = ph1;
				ph1 = ph1->next;
				delete ph2;
			}
			VecNormalize(&data->VertNorms[i]);
		}
	}

	if (data->face)
	{
		data->link->ObjectBegin(data->rc);

		data->defaultsurface = data->link->SurfaceAdd(data->rc);
		if (!data->defaultsurface)
		{
			data->err = ERR_MEM;
			return;
		}

		data->link->SurfaceName(data->rc, data->defaultsurface, "default");
		data->link->SurfaceDiffuse(data->rc, data->defaultsurface, 0.9f, 0.9f, 0.9f);
		data->link->SurfaceAmbient(data->rc, data->defaultsurface, 0.1f, 0.1f, 0.1f);
		data->link->SurfaceRefPhong(data->rc, data->defaultsurface, 49.f);

		triangle = data->link->TriangleAdd(data->rc, data->facecount,data->defaultsurface,data->mainactor);
		if (!triangle)
		{
			data->err = ERR_MEM;
			return;
		}
		if (data->link->type == LINK_SCENARIO)
		{  // modeler needs points,edges and faces seperate
			if (data->link->TriangleAddPoints(data->rc, data->pointcount,data->points) == -1)
			{
				data->err = ERR_MEM;
				return;
			}
			edges = new UWORD[data->facecount*6];
			if (!edges)
			{
				data->err = ERR_MEM;
				return;
			}
			for (i = 0; i < data->facecount; i++)
			{
				h = i*6;
				edges[h++] = data->face[i].p1;
				edges[h++] = data->face[i].p2;
				edges[h++] = data->face[i].p2;
				edges[h++] = data->face[i].p3;
				edges[h++] = data->face[i].p3;
				edges[h++] = data->face[i].p1;
			}
			if (data->link->TriangleAddEdges(data->rc, data->facecount*3,edges) == -1)
			{
				delete edges;
				data->err = ERR_MEM;
				return;
			}
			delete edges;
		}
		for (i = 0; i < data->facecount; i++)
		{
			p1 = data->face[i].p1;
			p2 = data->face[i].p3;
			p3 = data->face[i].p2;

			if(data->replacesurface)
				data->link->TriangleSurface(data->rc, triangle, data->replacesurface);
			else
				data->link->TriangleSurface(data->rc, triangle, data->material[i]);

			if (data->link->type == LINK_SCENARIO)
			{  // modeler needs edges
				data->link->TriangleSetEdges(data->rc, triangle,i*3,i*3+1,i*3+2);
			}
			else
			{
				// raystorm renderer needs triangles and normals
				data->link->TrianglePoints(data->rc, triangle,&data->points[p1],&data->points[p2],&data->points[p3]);

				if (!VecZero(data->TriNorms[i]))
				{
					// generate smooth triangle when smooth flag is set
					if (data->TriSmooth[i])
					{
						data->link->TriangleVNorm(data->rc, triangle,
							VecZero(data->VertNorms[p1]) ? &data->TriNorms[i] : &data->VertNorms[p1],
							VecZero(data->VertNorms[p2]) ? &data->TriNorms[i] : &data->VertNorms[p2],
							VecZero(data->VertNorms[p3]) ? &data->TriNorms[i] : &data->VertNorms[p3]);
					}
				}
				if(data->mapping)
				{
					data->link->TriangleUV(data->rc, triangle,
						&data->mapping[p1], &data->mapping[p2], &data->mapping[p3]);
				}
			}

			// next triangle
			triangle = data->link->TriangleGetNext(data->rc, triangle);
		}
		data->link->ObjectEnd(data->rc);
	}

	CleanupMesh(data);
}

/*************
 * DESCRIPTION: parse main data
 * INPUT:       pointer to chunk
 * OUTPUT:      -
 *************/
static void ParseMData(HANDLER_DATA *data, CHUNK *mainchunk)
{
	CHUNK chunk;

	do
	{
		BeginChunk(data, &chunk);
		switch (chunk.id)
		{
			case ID_PROPMATENTRY:
					ParsePropMat(data, &chunk);
					break;
			case ID_OBJECTDESC:
					ParseNamedObject(data, &chunk);
		}
		EndChunk(data, &chunk);
	}
	while (INCHUNK);
}

/*************
 * DESCRIPTION: parse a 3DS file
 * INPUT:       pointer to chunk
 * OUTPUT:      -
 *************/
static void Parse3DS(HANDLER_DATA *data, CHUNK *mainchunk)
{
	CHUNK chunk;

	do
	{
		BeginChunk(data, &chunk);
		switch (chunk.id)
		{
			case ID_MESHBLOCK:
				ParseMData(data, &chunk);
				break;
		}
		EndChunk(data, &chunk);
	}
	while (INCHUNK);
}

/*************
 * DESCRIPTION: read a 3DS-file
 * INPUT:       rc          context
 *              filename    name of 3DS-file
 *              link        link structure
 *              pos         object position
 *              ox, oy, oz  object orientation
 *              actor       pointer to actor
 *              replacesurf surface to replace object surface with
 *              version     version of file
 * OUTPUT:      NULL if ok, else error string
 *************/
extern "C" char* SAVEDS objRead(rsiCONTEXT *rc_, char* filename, OBJLINK *link_, const VECTOR *pos, const VECTOR *ox, const VECTOR *oy, const VECTOR *oz,
	const VECTOR *scale, ACTOR *actor, SURFACE *replacesurf, ULONG *version)
{
	CHUNK chunk;
	HANDLER_DATA data;

	data.link = link_;
	data.rc = rc_;
	data.pointcount = 0;
	data.points = NULL;
	data.mapping = NULL;
	data.VertNorms = NULL;
	data.face = NULL;
	data.facecount = 0;
	data.material = NULL;
	data.defaultsurface = NULL;
	data.replacesurface = NULL;
	data.smooth_angle = 0.5235987f;
	data.TriNorms = NULL;
	data.TriSmooth = NULL;
	data.TriList = NULL;
	data.cos_smooth_angle = cos(data.smooth_angle);
	data.mainactor = NULL;

	data.transmatrix.IdentityMatrix();
	data.matrix.SetSOTMatrix(scale, ox, oy, oz, pos);
	data.alignmatrix.SetOMatrix(ox, oy, oz);

	data.err = OK;
	data.hFile = Open3DS(filename);

	if (!data.hFile)
	{
		Cleanup3DS(&data);
		return errors[ERR_OPEN];
	}

	BeginChunk(&data, &chunk);

	// test if 3ds file
	if (chunk.id != ID_PRIMARY)
	{
		Cleanup3DS(&data);
		return errors[ERR_NO3DS];
	}

	data.mainactor = actor;
	data.replacesurface = replacesurf;

	Parse3DS(&data, &chunk);

	EndChunk(&data, &chunk);

	Cleanup3DS(&data);

	return errors[data.err];
}

/*************
 * DESCRIPTION:   cleanup all mesh resources
 * INPUT:       -
 * OUTPUT:      -
 *************/
static void CleanupMesh(HANDLER_DATA *data)
{
	if (data->TriList)
	{
		free(data->TriList);
		data->TriList = NULL;
	}
	if (data->TriSmooth)
	{
		delete [ ] data->TriSmooth;
		data->TriSmooth = NULL;
	}
	if (data->TriNorms)
	{
		delete [ ] data->TriNorms;
		data->TriNorms = NULL;
	}
	if (data->mapping)
	{
		delete [ ] data->mapping;
		data->mapping = NULL;
	}
	if (data->points)
	{
		delete [ ] data->points;
		data->points = NULL;
	}
	if (data->face)
	{
		delete [ ] data->face;
		data->face = NULL;
	}
	if (data->VertNorms)
	{
		delete [ ] data->VertNorms;
		data->VertNorms = NULL;
	}
	if (data->material)
	{
		free(data->material);
		data->material = NULL;
	}
}

/*************
 * DESCRIPTION: cleanup all allocated resources
 * INPUT:       -
 * OUTPUT:      -
 *************/
extern "C" void Cleanup3DS(HANDLER_DATA *data)
{
	if (data->hFile)
	{
		Close3DS(data);
		data->hFile = NULL;
	}
	CleanupMesh(data);
}

extern "C" void SAVEDS objCleanup()
{
}

