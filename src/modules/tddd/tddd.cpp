/***************
 * MODULE:        TDDD handler (Imagine 3.0 Object File Format)
 * NAME:          tddd.cpp
 * DESCRIPTION:   This module keeps all functions to read TDDD-files
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    29.01.96 ah    initial release
 *    05.03.96 ah    fixed bug (Imagine 4.0 wrote files slightly different)
 *    18.03.96 ah    world is no longer needed for readTDDD()
 *    20.03.96 ah    changed sphere and plane from SIZE to TOBJ
 *       added SetAxis() for modeller
 *    13.04.96 ah    had some problems with sharp edges. Now this should be
 *       handled correct, but it's much slower
 *    18.05.96 ah    added surf to readTDDD()
 *    11.07.96 ah    added suport of new TXT4-chunk from IM3.0
 *    03.10.96 ah    again fixed bugs in handling of sharp edges
 *    07.10.96 ah    EndObject() must be called in the TOBJ-section
 *       not in the DESC-section
 *    02.12.96 ah    sharp edges were set for non sharp edges
 *    20.01.97 ah    the position of the axis of a scaled object loaded to Scenario
 *       was not scaled
 *    08.02.97 ah    made some changes to make this module a shared library for AMIGA
 *    20.02.97 ah    crashed while loading empty axes to scenario
 *                   all axes had a wrong orientation (inverted)
 *    04.03.97 ah    improved calculation of triangle normals
 *    07.03.97 ah    moved normal calculation to raystorm.library
 *    14.03.97 mh    bugfix in ReadBrush, ReadFract converts to float now
 *    15.05.97 ah    added version
 *    23.10.97 ah    now only one surface for one object in Scenario,
 *       multiple objects only in renderer
 ***************/

#include <stdio.h>
#include <string.h>

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

#ifndef VECMATH_H
#include "vecmath.h"
#endif

#ifndef TDDD_H
#include "tddd.h"
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
class FLARE;
class STAR;
class OBJECT;
typedef void rsiCONTEXT;

#ifndef OBJLINK_H
#include "objlink.h"
#endif

enum errnums
{
	ERR_MEM,ERR_OPENFILE,ERR_TDDDFILE,ERR_IMTEXTURE,ERR_BRUSH
};

typedef struct tTMPBRUSHOBJ
{
	struct tTMPBRUSHOBJ *next;
	VECTOR pos;
	VECTOR orient_x, orient_y, orient_z;
	VECTOR size;
} TMPBRUSHOBJ;

typedef struct tTMPTEXTUREOBJ
{
	struct tTMPTEXTUREOBJ *next;
	VECTOR pos;
	VECTOR orient_x, orient_y, orient_z;
	VECTOR size;
} TMPTEXTUREOBJ;

typedef struct
{
	OBJLINK *link;
	IFFHandle *iff;
	FACE *face;
	VECTOR *points;
	EDGE *edge;
	VECTOR *norms;
	UBYTE *edgeflags;
	UBYTE (*clist)[3];
	UBYTE (*rlist)[3];
	UBYTE (*tlist)[3];
	TMPBRUSHOBJ *tmpbrush;
	TMPTEXTUREOBJ *tmptexture;
	rsiCONTEXT *rc;
	VECTOR pos,ox,oy,oz,scale;
	MATRIX matrix, alignmatrix;
} HANDLER_DATA;

static char *errors[] =
{
	"Not enough memory",
	"Can't open TDDD object file",
	"Corrupt TDDD file",
	"Can't load Imagine texture file",
	"Can't load brush file"
};

static void readTDDD_cleanup(HANDLER_DATA*);

#undef SAVEDS
#ifdef __PPC__
#define SAVEDS __saveds
#else
#define SAVEDS
#endif

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
 * FUNCTION:      ReadWord
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
 * FUNCTION:      ReadFract
 * DESCRIPTION:   Read fracts from iff-file
 * INPUT:         h        iffhandle
 *                buf      pointer to buffer
 *                cnt      number of fracts
 * OUTPUT:        FALSE if failed, else TRUE
 *************/
static BOOL ReadFract(IFFHandle *h, FRACT *buf, int cnt)
{
	int  i;

	if (!ReadChunkBytes(h, buf, sizeof(FRACT) * cnt) == sizeof(FRACT) * cnt)
		return FALSE;

#ifdef __SWAP__
	for(i = 0; i < cnt; i++)
		((float *)buf)[i] = float(LONG(Swap(buf[i])))*FRACT2FLOAT;
#else
	for(i = 0; i < cnt; i++)
		((float *)buf)[i] = float(LONG(buf[i]))*FRACT2FLOAT;
#endif

	return TRUE;
}

/*************
 * FUNCTION:      ReadColor
 * DESCRIPTION:   Read color from iff-file
 * INPUT:         iff      iffhandle
 *                col      color buffer
 * OUTPUT:        FALSE if failed, else TRUE
 *************/
static BOOL ReadColor(IFFHandle *iff, UBYTE color[3])
{
	ULONG col;

	if(ReadChunkBytes(iff,&col,4) != 4)
		return FALSE;

#ifdef __SWAP__
	col = Swap(col);
#endif

	color[0] = (UBYTE)CONVR(col);
	color[1] = (UBYTE)CONVG(col);
	color[2] = (UBYTE)CONVB(col);

	return TRUE;
}

/*************
 * FUNCTION:      FreeBuffers
 * DESCRIPTION:   free temporrary buffers
 * INPUT:         data     handler data
 * OUTPUT:        none
 *************/
static void FreeBuffers(HANDLER_DATA *data)
{
	if(data->points)
	{
		delete [ ] data->points;
		data->points = NULL;
	}
	if(data->edge)
	{
		delete [ ] data->edge;
		data->edge = NULL;
	}
	if(data->edgeflags)
	{
		delete [ ] data->edgeflags;
		data->edgeflags = NULL;
	}
	if(data->face)
	{
		delete [ ] data->face;
		data->face = NULL;
	}
	if(data->norms)
	{
		delete [ ] data->norms;
		data->norms = NULL;
	}
	if(data->clist)
	{
		delete [ ] data->clist;
		data->clist = NULL;
	}
	if(data->rlist)
	{
		delete [ ] data->rlist;
		data->rlist = NULL;
	}
	if(data->tlist)
	{
		delete [ ] data->tlist;
		data->tlist = NULL;
	}
}

/*************
 * FUNCTION:      FRACT2FLOAT
 * DESCRIPTION:   convert fract to float
 * INPUT:         f     fract
 * OUTPUT:        converted fract
 *************/
inline float fract2float(FRACT f)
{
#ifdef __SWAP__
	return float(LONG(Swap(f)))*FRACT2FLOAT;
#else
	return float(LONG(f))*FRACT2FLOAT;
#endif
}

/*************
 * FUNCTION:      HandleBrush
 * DESCRIPTION:   handle reading of brushes
 * INPUT:
 * OUTPUT:        error string
 *************/
static char *HandleBrush(HANDLER_DATA *data, SURFACE *surf, ACTOR *actor, const VECTOR *scale,
	VECTOR *main_pos, VECTOR *defmain_pos, VECTOR *x_axis, VECTOR *y_axis, VECTOR *z_axis, ULONG chunk)
{
	long rlen;
	IMAGINE_BRUSH i_brush;
	IMAGINE_BRUSH5 i_brush5;
	BRUSH *brush;
	char *name;
	ULONG flags;
	VECTOR size, orient_x, orient_y, orient_z, position;
	TMPBRUSHOBJ *tmpbrush, *tb;

	// Read brush
	if(chunk == ID_BRS5)
	{
		rlen = ReadChunkBytes(data->iff,&i_brush5,sizeof(IMAGINE_BRUSH5)-1);
		if (rlen != sizeof(IMAGINE_BRUSH5)-1)
			return errors[ERR_TDDDFILE];

		name = new char[i_brush5.Length+1];
		if (!name)
			return errors[ERR_MEM];

		name[i_brush5.Length] = '\0';

		// read brush name
		rlen = ReadChunkBytes(data->iff,name,i_brush5.Length);
		if (rlen != i_brush5.Length)
		{
			delete [ ] name;
			return errors[ERR_TDDDFILE];
		}
		i_brush = i_brush5.brush;
	}
	else
	{
		rlen = ReadChunkBytes(data->iff,&i_brush,sizeof(IMAGINE_BRUSH)-1);
		if (rlen != sizeof(IMAGINE_BRUSH)-1)
			return errors[ERR_TDDDFILE];

		name = new char[i_brush.Length+1];
		if (!name)
			return errors[ERR_MEM];

		name[i_brush.Length] = '\0';

		// read brush name
		rlen = ReadChunkBytes(data->iff,name,i_brush.Length);
		if (rlen != i_brush.Length)
		{
			delete [ ] name;
			return errors[ERR_TDDDFILE];
		}
	}

	brush = data->link->BrushAdd(data->rc, name,surf);
	if (!brush)
	{
		delete [ ] name;
		return errors[ERR_BRUSH];
	}

	delete [ ] name;

	// generate flags
	flags = 0;
	if(i_brush.Flags & IM_MAP_COLOR)
		flags = LINK_BRUSH_MAP_COLOR;
	if(i_brush.WFlags & IM_MAP_REFLECTIFITY)
		flags = LINK_BRUSH_MAP_REFLECTIFITY;
	if(i_brush.WFlags & IM_MAP_FILTER)
		flags = LINK_BRUSH_MAP_FILTER;
	if(i_brush.WFlags & IM_MAP_ALTITUDE)
		flags = LINK_BRUSH_MAP_ALTITUDE;
	if(i_brush.WFlags & IM_MAP_REFLECTION)
		flags = LINK_BRUSH_MAP_REFLECTION;

	if(i_brush.WFlags & IM_WRAP_X)
		flags |= LINK_BRUSH_WRAPX;
	if(i_brush.WFlags & IM_WRAP_Y)
		flags |= LINK_BRUSH_WRAPY;
	if(i_brush.WFlags & IM_REPEAT)
		flags |= LINK_BRUSH_REPEAT;
	if(i_brush.WFlags & IM_MIRROR)
		flags |= LINK_BRUSH_MIRROR;

	data->link->BrushFlags(data->rc, brush,flags);
	size.x = fract2float(i_brush.size.x)*scale->x;
	size.y = fract2float(i_brush.size.z)*scale->y;
	size.z = fract2float(i_brush.size.y)*scale->z;

	position.x = fract2float(i_brush.pos.x);
	position.y = fract2float(i_brush.pos.z);
	position.z = fract2float(i_brush.pos.y);

	switch(i_brush.Flags & IM_WRAP_MASK)
	{
		case IM_WRAP_XY:
		case IM_WRAP_X:
		case IM_WRAP_Y:
			// rotate orientation 90 degree clockwise because
			// Imagine handles brush in a other way
			orient_x.x = -fract2float(i_brush.x_axis.z);
			orient_x.y = fract2float(i_brush.x_axis.y);
			orient_x.z = fract2float(i_brush.x_axis.x);
			orient_y.x = -fract2float(i_brush.y_axis.z);
			orient_y.y = fract2float(i_brush.y_axis.y);
			orient_y.z = fract2float(i_brush.y_axis.x);
			orient_z.x = -fract2float(i_brush.z_axis.z);
			orient_z.y = fract2float(i_brush.z_axis.y);
			orient_z.z = fract2float(i_brush.z_axis.x);

			InvOrient(&orient_x, &orient_y, &orient_z, &orient_x, &orient_y, &orient_z);

			break;
		default:
			// flat mapping
			orient_x.x = fract2float(i_brush.x_axis.x);
			orient_x.y = fract2float(i_brush.x_axis.z);
			orient_x.z = fract2float(i_brush.x_axis.y);
			orient_y.x = fract2float(i_brush.z_axis.x);
			orient_y.y = fract2float(i_brush.z_axis.z);
			orient_y.z = fract2float(i_brush.z_axis.y);
			orient_z.x = fract2float(i_brush.y_axis.x);
			orient_z.y = fract2float(i_brush.y_axis.z);
			orient_z.z = fract2float(i_brush.y_axis.y);

			InvOrient(&orient_x, &orient_y, &orient_z, &orient_x, &orient_y, &orient_z);

			size.x *= .5;
			size.y *= .5;
			size.z *= .5;
			// translate axis to middle of brush
			position.x += dotp(&orient_x, &size);
			position.y += dotp(&orient_y, &size);
			position.z += dotp(&orient_z, &size);
			break;
	}
	tmpbrush = new TMPBRUSHOBJ;
	if(!tmpbrush)
		return errors[ERR_MEM];

	if(data->tmpbrush)
	{
		tb = data->tmpbrush;
		while(tb->next)
			tb = tb->next;
		tb->next = tmpbrush;
	}
	else
	{
		data->tmpbrush = tmpbrush;
	}

	tmpbrush->pos = position;
	tmpbrush->orient_x = orient_x;
	tmpbrush->orient_y = orient_y;
	tmpbrush->orient_z = orient_z;
	tmpbrush->size = size;

	return NULL;
}

/*************
 * FUNCTION:      HandleTexture
 * DESCRIPTION:   handle reading of textures
 * INPUT:         txt4     TRUE if this is a TXT4 (IM3.0) Texture
 * OUTPUT:        error string
 *************/
static char *HandleTexture(HANDLER_DATA *data, SURFACE *surf, ACTOR *actor, const VECTOR *scale,
	VECTOR *main_pos, VECTOR *defmain_pos, BOOL txt4)
{
	long rlen;
	IMAGINE_TEXTURE i_texture;
	TEXTURE *texture;
	char *name;
	int i;
	VECTOR size, orient_x, orient_y, orient_z, position;
	char buffer[22];
	TMPTEXTUREOBJ *tmptexture, *tt;

	if(!txt4)
	{
		// Read texture
		rlen = ReadChunkBytes(data->iff,&i_texture,sizeof(IMAGINE_TEXTURE)-1);
		if (rlen != sizeof(IMAGINE_TEXTURE)-1)
			return errors[ERR_TDDDFILE];
	}
	else
	{
		// Read texture
		rlen = ReadChunkBytes(data->iff,&i_texture,sizeof(IMAGINE_TEXTURE)-2);
		if (rlen != sizeof(IMAGINE_TEXTURE)-2)
			return errors[ERR_TDDDFILE];

		// skip 22 extra bytes
		rlen = ReadChunkBytes(data->iff,&buffer,22);
		if (rlen != 22)
			return errors[ERR_TDDDFILE];

		rlen = ReadChunkBytes(data->iff,&i_texture.Length,1);
		if (rlen != 1)
			return errors[ERR_TDDDFILE];
	}

	name = new char[i_texture.Length+5];
	if (!name)
		return errors[ERR_MEM];

	// read texture name
	rlen = ReadChunkBytes(data->iff,name,i_texture.Length);
	if (rlen != i_texture.Length)
	{
		delete [ ] name;
		return errors[ERR_TDDDFILE];
	}

	name[i_texture.Length] = '\0';

	strcat(name,".itx");
	texture = data->link->ITextureAdd(data->rc, name,surf);
	if(!texture)
	{
		delete name;
		return errors[ERR_IMTEXTURE];
	}
	delete name;

	for(i=0; i<16; i++)
		data->link->ITextureParams(data->rc, texture,i,fract2float(i_texture.Params[i]));

	position.x = fract2float(i_texture.pos.x);
	position.y = fract2float(i_texture.pos.z);
	position.z = fract2float(i_texture.pos.y);
	VecSub(&position,main_pos,&position);
	data->matrix.MultVectMat(&position);
	VecAdd(&position,main_pos,&position);

	orient_x.x = fract2float(i_texture.x_axis.x);
	orient_x.y = fract2float(i_texture.x_axis.z);
	orient_x.z = fract2float(i_texture.x_axis.y);
	orient_y.x = fract2float(i_texture.z_axis.x);
	orient_y.y = fract2float(i_texture.z_axis.z);
	orient_y.z = fract2float(i_texture.z_axis.y);
	orient_z.x = fract2float(i_texture.y_axis.x);
	orient_z.y = fract2float(i_texture.y_axis.z);
	orient_z.z = fract2float(i_texture.y_axis.y);
	InvOrient(&orient_x, &orient_y, &orient_z, &orient_x, &orient_y, &orient_z);

	data->alignmatrix.MultVectMat(&orient_x);
	data->alignmatrix.MultVectMat(&orient_y);
	data->alignmatrix.MultVectMat(&orient_z);

	size.x = fract2float(i_texture.size.x)*scale->x;
	size.y = fract2float(i_texture.size.z)*scale->y;
	size.z = fract2float(i_texture.size.y)*scale->z;

	tmptexture = new TMPTEXTUREOBJ;
	if(!tmptexture)
		return errors[ERR_MEM];

	if(data->tmptexture)
	{
		tt = data->tmptexture;
		while(tt->next)
			tt = tt->next;
		tt->next = tmptexture;
	}
	else
	{
		data->tmptexture = tmptexture;
	}

	tmptexture->pos = position;
	tmptexture->orient_x = orient_x;
	tmptexture->orient_y = orient_y;
	tmptexture->orient_z = orient_z;
	tmptexture->size = size;

	return NULL;
}

/*************
 * FUNCTION:      OpenIFF
 * DESCRIPTION:   open iff file and parse to begin of file
 * INPUT:         iff      iff handle
 *                filename name of file
 *                filesize   size of file is returned in this variable
 * OUTPUT:        error string
 *************/
static char *OpenIFF(struct IFFHandle **iff, char *filename, unsigned int *filesize)
{
	struct ContextNode *cn;

	// Allocate IFF_File structure.
	*iff = AllocIFF();
	if(!*iff)
		return errors[ERR_MEM];

	// Set up IFF_File for AmigaDOS I/O.
#ifdef __AMIGA__
	(*iff)->iff_Stream = Open(filename, MODE_OLDFILE);
#else
	(*iff)->iff_Stream = Open_(filename, MODE_OLDFILE);
#endif
	if(!(*iff)->iff_Stream)
		return errors[ERR_OPENFILE];

	InitIFFasDOS(*iff);

	// Start the IFF transaction.
	if(OpenIFF(*iff, IFFF_READ))
		return errors[ERR_TDDDFILE];

	if(ParseIFF(*iff, IFFPARSE_RAWSTEP))
		return errors[ERR_TDDDFILE];

	cn = CurrentChunk(*iff);
	*filesize = cn->cn_Size;

	// get OBJ-chunk
	if(ParseIFF(*iff, IFFPARSE_RAWSTEP))
		return errors[ERR_TDDDFILE];

	cn = CurrentChunk(*iff);
	if(cn->cn_ID != ID_OBJ)
		return errors[ERR_TDDDFILE];
	cn->cn_Size = 0;

	return NULL;
}

/*************
 * FUNCTION:      AddMesh
 * DESCRIPTION:   add a mesh object
 * INPUT:         surf        surface
 *                replacesurf surface to replace object surface with
 *                actor       actor
 *                pointcount  amount of points
 *                edgecount   amount of edges
 *                facecount   amount of faces
 *                pos_axis    position axis
 *                ox_axis,oy_axis,oz_axis    orientation axis
 *                size_axis   size axis
 *                diffuse     diffuse color
 *                reflect     reflective color
 *                transpar    transparent color
 *                i_props     object properties
 * OUTPUT:        error string
 *************/
static char *AddMesh(HANDLER_DATA *data, SURFACE *surf, SURFACE *replacesurf, ACTOR *actor, int pointcount, int edgecount, int facecount,
	VECTOR *pos_axis, VECTOR *ox_axis, VECTOR *oy_axis, VECTOR *oz_axis, VECTOR *size_axis,
	UBYTE *diffuse, UBYTE *reflect, UBYTE *transpar, IMAGINE_PROPS *i_props, OBJECT **obj)
{
	int i;
	LINK_EDGE *edges, *le;
	EDGE *e;
	SURFACE **surfs=NULL, *newsurf=NULL;
	VECTOR pos,ox,oy,oz,size;

	if(data->face)
	{
		edges = new LINK_EDGE[edgecount];
		if(!edges)
			return errors[ERR_MEM];

		e = data->edge;
		le = edges;
		for(i=0; i<edgecount; i++)
		{
			le->p[0] = e->begin;
			le->p[1] = e->end;
			if(data->edgeflags && i_props->phong)
			{
				if(data->edgeflags[i] & EDGE_SHARP)
					le->flags = LINK_EDGE_SHARP;
				else
					le->flags = 0;
			}
			else
			{
				if(i_props->phong)
					le->flags = 0;
				else
					le->flags = LINK_EDGE_SHARP;
			}
			e++;
			le++;
		}

		delete data->edge;
		data->edge = NULL;

		if(data->link->type == LINK_SCENARIO)
		{
			*obj = data->link->MeshCreate(data->rc);
			if(!*obj)
				return errors[ERR_MEM];

			data->link->ObjectAxis(data->rc, *obj, pos_axis, ox_axis,oy_axis,oz_axis, size_axis);

			if(!data->link->MeshAddScenario(data->rc, *obj,
				data->points, pointcount,
				edges, edgecount,
				(LINK_MESH*)data->face, facecount,
				&data->scale,
				!i_props->phong))
			{
				if(surfs)
					delete surfs;
				delete edges;
				return errors[ERR_MEM];
			}
		}
		else
		{
			if(!replacesurf && (data->clist || data->rlist || data->tlist))
			{
				// create new surface if no replacement
				// surface is given and definition in
				// colorlists differs from original
				// surface
				surfs = new SURFACE*[facecount];
				if(!surfs)
					return errors[ERR_MEM];

				for(i=0; i<facecount; i++)
				{
					surfs[i] = surf;
					if(data->clist)
					{
						if(data->clist[i][0] != diffuse[0] ||
							data->clist[i][1] != diffuse[1] ||
							data->clist[i][2] != diffuse[2])
						{
							// Create new surface
							newsurf = data->link->SurfaceAdd(data->rc);
							// Copy surface
							data->link->SurfaceCopy(data->rc, surf,newsurf);
							data->link->SurfaceDiffuse(data->rc, newsurf,
									float(data->clist[i][0])/255.f,
									float(data->clist[i][1])/255.f,
									float(data->clist[i][2])/255.f);
							surfs[i] = newsurf;
						}
					}
					if(data->rlist)
					{
						if(data->rlist[i][0] != reflect[0] ||
							data->rlist[i][1] != reflect[1] ||
							data->rlist[i][2] != reflect[2])
						{
							if(!newsurf)
							{
								// Create new surface
								newsurf = data->link->SurfaceAdd(data->rc);
								// Copy surface
								data->link->SurfaceCopy(data->rc, surf,newsurf);
								surfs[i] = newsurf;
							}
							data->link->SurfaceReflect(data->rc, newsurf,
								float(data->rlist[i][0])/255.f,
								float(data->rlist[i][1])/255.f,
								float(data->rlist[i][2])/255.f);
						}
					}
					if(data->tlist)
					{
						if(data->tlist[i][0] != transpar[0] ||
							data->tlist[i][1] != transpar[1] ||
							data->tlist[i][2] != transpar[2])
						{
							if(!newsurf)
							{
								// Create new surface
								newsurf = data->link->SurfaceAdd(data->rc);
								// Copy surface
								data->link->SurfaceCopy(data->rc, surf,newsurf);
								surfs[i] = newsurf;
							}
							data->link->SurfaceTranspar(data->rc, newsurf,
								float(data->tlist[i][0])/255.f,
								float(data->tlist[i][1])/255.f,
								float(data->tlist[i][2])/255.f);
						}
					}
					newsurf = NULL;
				}
			}
			SetVector(&pos, 0.f, 0.f, 0.f);
			SetVector(&ox, 1.f, 0.f, 0.f);
			SetVector(&oy, 0.f, 1.f, 0.f);
			SetVector(&oz, 0.f, 0.f, 1.f);
			SetVector(&size, 1.f, 1.f, 1.f);
			*obj = data->link->MeshAddRenderer(data->rc, surf, surfs, actor,
				data->points, pointcount,
				edges, edgecount,
				(LINK_MESH*)data->face, facecount,
				&pos, &ox,&oy,&oz, &size,
				!i_props->phong);
			if(!*obj)
			{
				if(surfs)
					delete surfs;
				delete edges;
				return errors[ERR_MEM];
			}
		}

		if(surfs)
			delete surfs;
		delete edges;
	}
	else
	{
		if(data->link->type == LINK_SCENARIO)
		{
			*obj = data->link->MeshCreate(data->rc);
			if(!*obj)
				return errors[ERR_MEM];

			data->link->ObjectAxis(data->rc, *obj, pos_axis, ox_axis, oy_axis, oz_axis, size_axis);
		}
	}

	return NULL;
}

/*************
 * FUNCTION:      objRead
 * DESCRIPTION:   read a TDDD-file
 * INPUT:         rc          context
 *                filename    name of TDDD-file
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
	int i;
	long error = 0;
	struct ContextNode *cn;
	UWORD pointcount,facecount,edgecount,edgeflagcount;
	WORD shape,lamp;
	VECTOR *p;
	SURFACE *surf;
	ULONG tmpcol;
	float temp;
	char *err;
	FRACT tmp;
	COLOR color;
	UBYTE diffuse[3],reflect[3],transpar[3];
	BOOL first_object = TRUE;  // TRUE for first object axis
	char name[18];
	IMAGINE_PROPS i_props;
	IMAGINE_AXIS i_axis;
	VECTOR defmain_pos, main_pos, v;
	VECTOR x_axis, y_axis, z_axis, pos_axis, size_axis;
	HANDLER_DATA data;
	OBJECT *obj;
	TMPBRUSHOBJ *tbrush, *tbrushnext;
	BRUSH_OBJECT *brushobj;
	TMPTEXTUREOBJ *ttexture, *ttexturenext;
	TEXTURE_OBJECT *textureobj;
	unsigned int filesize, size_done = 0;

	data.face = NULL;
	data.points = NULL;
	data.edge = NULL;
	data.norms = NULL;
	data.edgeflags = NULL;
	data.clist = NULL;
	data.rlist = NULL;
	data.tlist = NULL;
	data.link = link;
	data.tmpbrush = NULL;
	data.tmptexture = NULL;
	data.rc = rc;
	data.pos = *pos;
	data.ox = *ox;
	data.oy = *oy;
	data.oz = *oz;
	data.scale = *scale;

	data.matrix.SetSOTMatrix(scale, ox, oy, oz, pos);
	data.alignmatrix.SetOMatrix(ox, oy, oz);

	err = OpenIFF(&data.iff, filename, &filesize);
	if(err)
	{
		readTDDD_cleanup(&data);
		return err;
	}
	pos_axis.x = INFINITY;

	while (!error || error == IFFERR_EOC)
	{
		error = ParseIFF(data.iff, IFFPARSE_RAWSTEP);
		if (error != IFFERR_EOC)
		{
			// Get a pointer to the context node describing the current context
			cn = CurrentChunk(data.iff);
			if (cn)
			{
				size_done += cn->cn_Size;
				switch (cn->cn_ID)
				{
					case ID_OBJ:
						cn->cn_Size = 0;
						break;
					case ID_DESC:
					case ID_TOBJ:
						if(pos_axis.x != INFINITY)
						{
							obj = NULL;
							switch(shape)
							{
								case SHAPE_GROUND:
									obj = link->PlaneAdd(data.rc);
									if(!obj)
									{
										readTDDD_cleanup(&data);
										return errors[ERR_MEM];
									}
									if(link->type == LINK_RENDERER)
										link->PlaneAxis(data.rc, obj, &pos_axis, &y_axis);
									break;
								case SHAPE_SPHERE:
									obj = link->SphereAdd(data.rc);
									if(!obj)
									{
										readTDDD_cleanup(&data);
										return errors[ERR_MEM];
									}
									if(link->type == LINK_RENDERER)
										link->SphereAxis(data.rc, obj, &pos_axis, size_axis.x);
									break;
								case SHAPE_AXIS:
									err = AddMesh(&data, surf, replacesurf, actor, pointcount, edgecount, facecount,
										&pos_axis, &x_axis, &y_axis, &z_axis, &size_axis,
										diffuse, reflect, transpar, &i_props, &obj);
									if(err)
									{
										readTDDD_cleanup(&data);
										return err;
									}
									FreeBuffers(&data);
									break;
							}
							if(obj)
							{
								link->ObjectSurface(data.rc, obj, surf);
								if(link->type == LINK_SCENARIO)
								{
									if(shape != SHAPE_AXIS)
										link->ObjectAxis(data.rc, obj, &pos_axis,&x_axis,&y_axis,&z_axis,&size_axis);
									if(!link->ObjectName(data.rc, obj, name))
									{
										readTDDD_cleanup(&data);
										return errors[ERR_MEM];
									}
								}
								else
								{
									link->ObjectActor(data.rc, obj, actor);
								}
								brushobj = NULL;
								tbrush = data.tmpbrush;
								while(tbrush)
								{
									brushobj = data.link->BrushObjectAdd(data.rc, obj, actor, brushobj);
									if(!brushobj)
									{
										readTDDD_cleanup(&data);
										return errors[ERR_MEM];
									}
									VecAdd(&pos_axis, &tbrush->pos, &v);

									// modify brush position
									VecSub(&v, &defmain_pos, &v);
									data.matrix.MultVectMat(&v);
									VecAdd(&v, &main_pos, &v);
									data.link->BrushPos(data.rc, brushobj, &v);

									// brush orientation is local to object
									// -> make it global
									v = tbrush->orient_x;
									tbrush->orient_x.x = dotp(&x_axis, &v);
									tbrush->orient_x.y = dotp(&y_axis, &v);
									tbrush->orient_x.z = dotp(&z_axis, &v);
									v = tbrush->orient_y;
									tbrush->orient_y.x = dotp(&x_axis, &v);
									tbrush->orient_y.y = dotp(&y_axis, &v);
									tbrush->orient_y.z = dotp(&z_axis, &v);
									v = tbrush->orient_z;
									tbrush->orient_z.x = dotp(&x_axis, &v);
									tbrush->orient_z.y = dotp(&y_axis, &v);
									tbrush->orient_z.z = dotp(&z_axis, &v);

									// rotate brush orientation with user specifications
									data.alignmatrix.MultVectMat(&tbrush->orient_x);
									data.alignmatrix.MultVectMat(&tbrush->orient_y);
									data.alignmatrix.MultVectMat(&tbrush->orient_z);
									data.link->BrushOrient(data.rc, brushobj, &tbrush->orient_x,&tbrush->orient_y,&tbrush->orient_z);

									data.link->BrushSize(data.rc, brushobj, &tbrush->size);
									if(data.link->type == LINK_RENDERER)
									{
										// calculate alignment (thank you veeeery much Mike!)
//                            v.x = (float)asin(tbrush->orient_z.y);
//                            v.y = (float)atan2(tbrush->orient_z.x, tbrush->orient_z.z);
//                            v.z = (float)atan2(tbrush->orient_x.y, tbrush->orient_y.y);
										Orient2Angle(&v, &tbrush->orient_x, &tbrush->orient_y, &tbrush->orient_z);
										data.link->BrushAlignment(data.rc, brushobj, &v);
									}

									tbrushnext = tbrush->next;
									delete tbrush;
									tbrush = tbrushnext;
								}
								data.tmpbrush = NULL;
								textureobj = NULL;
								ttexture = data.tmptexture;
								while(ttexture)
								{
									textureobj = data.link->TextureObjectAdd(data.rc, obj, actor, textureobj);
									if(!textureobj)
									{
										readTDDD_cleanup(&data);
										return errors[ERR_MEM];
									}
									data.link->TexturePos(data.rc, textureobj, &ttexture->pos);
									data.link->TextureOrient(data.rc, textureobj, &ttexture->orient_x,&ttexture->orient_y,&ttexture->orient_z);
									data.link->TextureSize(data.rc, textureobj, &ttexture->size);

									ttexturenext = ttexture->next;
									delete ttexture;
									ttexture = ttexturenext;
								}
								data.tmptexture = NULL;
							}
							pos_axis.x = INFINITY;
						}
						if(cn->cn_ID == ID_DESC)
						{
							cn->cn_Size = 0;
							strcpy(name, "Unknown");
							link->ObjectBegin(data.rc);

							if(!replacesurf)
								surf = link->SurfaceAdd(data.rc);
							else
								surf = replacesurf; // replace object surface

							pos_axis.x = INFINITY;
						}
						else
							link->ObjectEnd(data.rc);
						break;
					case ID_NAME:
						if(replacesurf)
							break;
						// Read name
						if(ReadChunkBytes(data.iff,name,18) != 18)
						{
							readTDDD_cleanup(&data);
							return errors[ERR_TDDDFILE];
						}
						if(!link->SurfaceName(data.rc, surf, name))
						{
							readTDDD_cleanup(&data);
							return errors[ERR_MEM];
						}
						break;
					case ID_SHAP:
					case ID_SHP2:
						// Read shape
						if (!ReadWord(data.iff, &shape, 1))
						{
							readTDDD_cleanup(&data);
							return errors[ERR_TDDDFILE];
						}
						// Read lamp
						if (!ReadWord(data.iff, &lamp, 1))
						{
							readTDDD_cleanup(&data);
							return errors[ERR_TDDDFILE];
						}
						if(replacesurf)
							break;
						if(lamp & LAMP_BRIGHT)
							link->SurfaceSetFlags(data.rc, surf,LINK_SURFACE_BRIGHT);
						break;
					case ID_PNTS:
						// Read point count
						if (!ReadWord(data.iff, (WORD *)&pointcount, 1))
						{
							readTDDD_cleanup(&data);
							return errors[ERR_TDDDFILE];
						}
						// Allocate memory
						data.points = new VECTOR[pointcount];
						if (!data.points)
						{
							readTDDD_cleanup(&data);
							return errors[ERR_MEM];
						}
						// Read points
						if (!ReadFract(data.iff,(FRACT *)data.points,pointcount*3))
						{
							readTDDD_cleanup(&data);
							return errors[ERR_TDDDFILE];
						}

						if(pointcount)
						{
							p = data.points;
							for(i=0; i<pointcount; i++)
							{
								temp = p->z;
								p->z = p->y;
								p->y = temp;
								// Translate, scale and rotate point
								VecSub(p, &defmain_pos, p);
								data.matrix.MultVectMat(p);
								VecAdd(p, &main_pos, p);
								p++;
							}
						}
						break;
					case ID_EDGE:
						// Read edge count
						if (!ReadWord(data.iff, (WORD *)&edgecount, 1))
						{
							readTDDD_cleanup(&data);
							return errors[ERR_TDDDFILE];
						}
						// Allocate memory
						data.edge = new EDGE[edgecount];
						if (!data.edge)
						{
							readTDDD_cleanup(&data);
							return errors[ERR_MEM];
						}
						// Read edges
						if (!ReadWord(data.iff, (WORD *)data.edge, edgecount * 2))
						{
							readTDDD_cleanup(&data);
							return errors[ERR_TDDDFILE];
						}
						break;
					case ID_EFLG:
						// Read edgeflag count
						if (!ReadWord(data.iff, (WORD *)&edgeflagcount, 1))
						{
							readTDDD_cleanup(&data);
							return errors[ERR_TDDDFILE];
						}
						// Allocate memory
						data.edgeflags = new UBYTE[edgeflagcount];
						if (!data.edgeflags)
						{
							readTDDD_cleanup(&data);
							return errors[ERR_MEM];
						}
						// Read edgeflags
						if (ReadChunkBytes(data.iff,data.edgeflags,edgeflagcount) != edgeflagcount)
						{
							readTDDD_cleanup(&data);
							return errors[ERR_TDDDFILE];
						}
						break;
					case ID_FACE:
						// Read face count
						if (!ReadWord(data.iff, (WORD *)&facecount, 1))
						{
							readTDDD_cleanup(&data);
							return errors[ERR_TDDDFILE];
						}
						// Allocate memory
						data.face = new FACE[facecount];
						if (!data.face)
						{
							readTDDD_cleanup(&data);
							return errors[ERR_MEM];
						}
						// Read faces
						if(!ReadWord(data.iff,(WORD *)data.face, 3 * facecount))
						{
							readTDDD_cleanup(&data);
							return errors[ERR_TDDDFILE];
						}
						break;
					case ID_SIZE:
						// Read object size
						if (!ReadFract(data.iff, (FRACT *)&size_axis, 3))
						{
							readTDDD_cleanup(&data);
							return errors[ERR_TDDDFILE];
						}
#ifdef __STORM__
						// once again one of this damned StormC Optimize bugs, bull shit!
						v = size_axis;
						size_axis.x = v.x * scale->x;
						size_axis.y = v.z * scale->y;
						size_axis.z = v.y * scale->z;
#else
						size_axis.x *= scale->x;
						temp = size_axis.z;
						size_axis.z = size_axis.y*scale->z;
						size_axis.y = temp*scale->y;
#endif // __STORM__
						break;
					case ID_POSI:
						// Read object position
						if (!ReadFract(data.iff, (FRACT *)&pos_axis, 3))
						{
							readTDDD_cleanup(&data);
							return errors[ERR_TDDDFILE];
						}
#ifdef __STORM__
						// once again one of this damned StormC Optimize bugs, bull shit!
						v = pos_axis;
						pos_axis.y = v.z;
						pos_axis.z = v.y;
#else
						temp = pos_axis.z;
						pos_axis.z = pos_axis.y;
						pos_axis.y = temp;
#endif // __STORM__
						if(first_object)
						{
							defmain_pos = pos_axis;    // we need the first position
							SetVector(&pos_axis, pos_axis.x*scale->x, pos_axis.y*scale->y, pos_axis.z*scale->z);
							main_pos = pos_axis;
							first_object = FALSE;
						}
						else
						{
							VecSub(&pos_axis, &defmain_pos, &pos_axis);
							SetVector(&pos_axis, pos_axis.x*scale->x, pos_axis.y*scale->y, pos_axis.z*scale->z);
							VecAdd(&pos_axis, &main_pos, &pos_axis);
						}
						break;
					case ID_AXIS:
						// Read object properties
						if (!ReadFract(data.iff, (FRACT *)&i_axis, 9))
						{
							readTDDD_cleanup(&data);
							return errors[ERR_TDDDFILE];
						}
						x_axis.x = *((float*)(&i_axis.x_axis.x));
						x_axis.y = *((float*)(&i_axis.x_axis.z));
						x_axis.z = *((float*)(&i_axis.x_axis.y));
						y_axis.x = *((float*)(&i_axis.z_axis.x));
						y_axis.y = *((float*)(&i_axis.z_axis.z));
						y_axis.z = *((float*)(&i_axis.z_axis.y));
						z_axis.x = *((float*)(&i_axis.y_axis.x));
						z_axis.y = *((float*)(&i_axis.y_axis.z));
						z_axis.z = *((float*)(&i_axis.y_axis.y));

						InvOrient(&x_axis, &y_axis, &z_axis, &x_axis, &y_axis, &z_axis);
						break;
					case ID_PRP1:
						// Read object properties
						if (ReadChunkBytes(data.iff,&i_props,sizeof(IMAGINE_PROPS)) != sizeof(IMAGINE_PROPS))
						{
							readTDDD_cleanup(&data);
							return errors[ERR_TDDDFILE];
						}
						if(replacesurf)
							break;
						// index of refraction
						link->SurfaceIndOfRefr(data.rc, surf,float(i_props.index)/100.0 + 1.0);
						link->SurfaceRefPhong(data.rc, surf,i_props.hardness);
						break;
					case ID_TXT3:
					case ID_TXT4:
						if(replacesurf)
							break;
						err = HandleTexture(&data, surf,actor,scale,&main_pos, &defmain_pos,
							cn->cn_ID == ID_TXT4);
						if(err)
						{
								readTDDD_cleanup(&data);
								return err;
						}
						break;
					case ID_BRS5:
					case ID_BRS4:
					case ID_BRS3:
						if(replacesurf)
							break;
						err = HandleBrush(&data, surf,actor,scale,&main_pos, &defmain_pos,
							&x_axis,&y_axis,&z_axis, cn->cn_ID);
						if(err)
						{
							readTDDD_cleanup(&data);
							return err;
						}
						break;
					case ID_COLR:
						if(replacesurf)
							break;
						// Read color
						if(!ReadColor(data.iff,diffuse))
						{
							readTDDD_cleanup(&data);
							return errors[ERR_TDDDFILE];
						}
						color.r = (float)(diffuse[0])/255;
						color.g = (float)(diffuse[1])/255;
						color.b = (float)(diffuse[2])/255;
						link->SurfaceDiffuse(data.rc, surf,color.r,color.g,color.b);
						link->SurfaceAmbient(data.rc, surf,color.r,color.g,color.b);
						break;
					case ID_FOGL:
						if(replacesurf)
							break;
						// Read fog length
						if(!ReadFract(data.iff,&tmp,1))
						{
							readTDDD_cleanup(&data);
							return errors[ERR_TDDDFILE];
						}
						link->SurfaceFoglen(data.rc, surf, tmp);
						break;
					case ID_CLST:
						if(replacesurf)
							break;
						// Read face count
						if (!ReadWord(data.iff, (WORD *)&facecount, 1))
						{
							readTDDD_cleanup(&data);
							return errors[ERR_TDDDFILE];
						}
						// Allocate memory
						data.clist = new UBYTE[facecount][3];
						if (!data.clist)
						{
							readTDDD_cleanup(&data);
							return errors[ERR_MEM];
						}
						// Read color list
						if(ReadChunkBytes(data.iff, data.clist, 3*facecount) != 3*facecount)
						{
							readTDDD_cleanup(&data);
							return errors[ERR_TDDDFILE];
						}
						break;
					case ID_REFL:
						if(replacesurf)
							break;
						// Read reflect
						if(!ReadColor(data.iff,reflect))
						{
							readTDDD_cleanup(&data);
							return errors[ERR_TDDDFILE];
						}
						link->SurfaceReflect(data.rc, surf,(float)(reflect[0])/255,
							(float)(reflect[1])/255,(float)(reflect[2])/255);
						break;
					case ID_RLST:
						if(replacesurf)
							break;
						// Read face count
						if (!ReadWord(data.iff, (WORD *)&facecount, 1))
						{
							readTDDD_cleanup(&data);
							return errors[ERR_TDDDFILE];
						}
						// Allocate memory
						data.rlist = new UBYTE[facecount][3];
						if (!data.rlist)
						{
							readTDDD_cleanup(&data);
							return errors[ERR_MEM];
						}
						// Read reflect list
						if(ReadChunkBytes(data.iff, data.rlist, 3*facecount) != 3*facecount)
						{
							readTDDD_cleanup(&data);
							return errors[ERR_TDDDFILE];
						}
						break;
					case ID_TRAN:
						if(replacesurf)
							break;
						// Imagine uses COLOR for the fog color, we use transparence
						// -> read transparence if foglength is zero else take COLOR
						if(link->SurfaceGetFoglen(data.rc, surf) == 0.)
						{
							// Read transparence
							if(!ReadColor(data.iff, transpar))
							{
								readTDDD_cleanup(&data);
								return errors[ERR_TDDDFILE];
							}
						}
						else
						{
							transpar[0] = diffuse[0];
							transpar[1] = diffuse[1];
							transpar[2] = diffuse[2];
						}
						link->SurfaceTranspar(data.rc, surf,(float)(transpar[0])/255,
							(float)(transpar[1])/255,(float)(transpar[2])/255);
						break;
					case ID_TLST:
						if(replacesurf)
							break;
						// Read face count
						if (!ReadWord(data.iff, (WORD *)&facecount, 1))
						{
							readTDDD_cleanup(&data);
							return errors[ERR_TDDDFILE];
						}
						// Allocate memory
						data.tlist = new UBYTE[facecount][3];
						if (!data.tlist)
						{
							readTDDD_cleanup(&data);
							return errors[ERR_MEM];
						}
						// Imagine uses COLOR for the fog color, we use transparence
						// -> read transparence if foglength is zero else take COLOR
						if(link->SurfaceGetFoglen(data.rc, surf) == 0.)
						{
							// Read transpar list
							if(ReadChunkBytes(data.iff, data.tlist, 3*facecount) != 3*facecount)
							{
								readTDDD_cleanup(&data);
								return errors[ERR_TDDDFILE];
							}
						}
						else
						{
							// copy color to transparence
							for(i=0; i<facecount; i++)
							{
								data.tlist[i][0] = data.clist[i][0];
								data.tlist[i][1] = data.clist[i][1];
								data.tlist[i][2] = data.clist[i][2];
							}
						}
						break;
					case ID_SPC1:
						if(replacesurf)
							break;
						// Read specular
						if (ReadChunkBytes(data.iff,&tmpcol,4) != 4)
						{
							readTDDD_cleanup(&data);
							return errors[ERR_TDDDFILE];
						}
						link->SurfaceSpecular(data.rc, surf,(float)(CONVR(tmpcol))/255,
							(float)(CONVG(tmpcol))/255,(float)(CONVB(tmpcol))/255);
						break;
				}
				if(SetProgress)
					(*SetProgress)(rc, (float)size_done/(float)filesize);
			}
		}
	}
	readTDDD_cleanup(&data);
	if (error != IFFERR_EOF)
	{
		return errors[ERR_TDDDFILE];
	}
	return NULL;
}

/*************
 * FUNCTION:      readTDDD_cleanup
 * DESCRIPTION:   free mem and iff-handle
 * INPUT:         data     handler data
 * OUTPUT:        none
 *************/
static void readTDDD_cleanup(HANDLER_DATA *data)
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
 * FUNCTION:      objCleanup
 * DESCRIPTION:   Cleanup TDDD handler
 * INPUT:         none
 * OUTPUT:        none
 *************/
extern "C" void SAVEDS objCleanup()
{
}

