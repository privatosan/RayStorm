/***************
 * MODUL:         RSOB handler (RayStorm Object File Format)
 * NAME:          rsob.cpp
 * DESCRIPTION:   In this module are all the functions to read RSOB-files
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    20.10.96 ah    initial release
 *    29.11.96 ah    added ParseFlare() and ParseStar()
 *    20.01.97 ah    the axis of textures and brushes where pushed to the
 *       matrix stack but never poped; therefore all objects after a texture
 *       had a wrong axis
 *    25.01.97 ah    added ParseBox()
 *    31.01.97 ah    added tilt angle to star
 *    03.03.97 ah    improved normal calculation of triangles
 *    06.03.97 ah    added shadowmapsize to lights
 *    07.03.97 ah    moved normal calculation of meshes to rsi
 *    26.03.97 ah    added ParseCylinder()
 *    26.03.97 ah    added ParseCone()
 *    15.05.97 ah    added version
 *    16.05.97 ah    changed handling of surfaces to new V2 style
 *    08.06.97 ah    changed handling of brushes to new V2 style
 *                   made handler reentrant
 *    25.09.97 ah    added ParseSOR()
 *    06.09.87 ah    added directional light, raystorm textures
 ***************/

#include <stdio.h>
#include <string.h>

#ifndef __AMIGA__
	#include "iffparse.h"
#else
	#include <utility/tagitem.h>
	#include <libraries/iffparse.h>
	#include <exec/memory.h>
	#include <pragma/iffparse_lib.h>
	#include <pragma/dos_lib.h>
	#include <pragma/exec_lib.h>
	#include "rmod:objlib.h"
	#pragma libbase ObjHandBase;
	extern "ASM" void abort__STANDARD(){};
#endif

#ifndef VECMATH_H
#include "vecmath.h"
#endif

#ifndef COLOR_H
#include "color.h"
#endif

#ifndef IFF_UTIL_H
#include "iff_util.h"
#endif

#ifndef BRUSH_H
#include "brush.h"
#endif

#ifndef RSCN_H
#include "rscn.h"
#endif

typedef struct
{
	UWORD p[2];
	UWORD flags;
} EDGE;

typedef struct
{
	UWORD e[3];
} TRIANGLE;

class ACTOR;
class SURFACE;
class TEXTURE;
class TEXTURE_OBJECT;
class BRUSH;
class BRUSH_OBJECT;
class CAMERA;
class LIGHT;
class FLARE;
class STAR;
class OBJECT;

#ifndef OBJLINK_H
#include "objlink.h"
#endif

enum
{
	ERR_MEM,ERR_OPENFILE,ERR_IFFPARSE,ERR_NORSOBFILE,ERR_RSOBFILE,ERR_IMTEXTURE,
	ERR_WRONGVERS, ERR_READFILE, ERR_BRUSH, ERR_TEXTURE
};

static char *errors[] =
{
	"Not enough memory",
	"Can't open RSOB object file",
	"Error in iffparse. Corrupt file.",
	"No RSOB file",
	"Corrupt RSOB file",
	"Can't load texture file",
	"Too new version of object file. Please install new handler.",
	"Error reading file",
	"Can't load brush",
	"Can't load texture"
};

typedef struct
{
	OBJLINK *link;
	int errcode;
	ULONG rsob_version;
	IFFHandle *iff;
	BRUSH_OBJECT *prevbrush;
	TEXTURE_OBJECT *prevtexture;
	rsiCONTEXT *rc;
	VECTOR pos,ox,oy,oz,scale;
	ULONG size_done, filesize;
} HANDLER_DATA;

// Prototypes
static SURFACE *ParseObjectSurface(HANDLER_DATA*, MATRIX_STACK*, OBJECT*, ACTOR*);
static SURFACE *ParseSurface(HANDLER_DATA*, MATRIX_STACK*, OBJECT*, ACTOR*);
static void ReadRSOB_Cleanup(HANDLER_DATA*);

#undef SAVEDS
#ifdef __PPC__
#define SAVEDS __saveds
#else
#define SAVEDS
#endif

/*************
 * DESCRIPTION:   Parse a SURF chunk
 * INPUT:         data     handler data
 *                stack    matrix stack
 *                obj      object
 *                actor    actor
 * OUTPUT:        FALSE if failed
 *************/
static SURFACE *HandleSurface(HANDLER_DATA *data, MATRIX_STACK *stack, OBJECT *obj, ACTOR *actor)
{
	SURFACE *surf;

	if(data->rsob_version < 200)
	{
		surf = ParseSurface(data, stack, obj, actor);
		if(!surf)
		{
			data->errcode = ERR_MEM;
			return NULL;
		}
	}
	else
	{
		surf = ParseObjectSurface(data, stack, obj, actor);
		if(!surf)
		{
			data->errcode = ERR_MEM;
			return NULL;
		}
	}
	return surf;
}

/*************
 * DESCRIPTION:   Read flags for objects
 * INPUT:         data     handler data
 *                obj      object
 *                flags    flags
 * OUTPUT:        FALSE if failed
 *************/
static BOOL ReadFlags(HANDLER_DATA *data, OBJECT *obj, ULONG *flags_=NULL)
{
	ULONG f, flags=0;

	if(!ReadULONG(data->iff,&f,1))
		return FALSE;

	if(f & RSCN_OBJECT_NODEOPEN)
		flags |= LINK_OBJECT_NODEOPEN;
	if(f & RSCN_OBJECT_INVERTED)
		flags |= LINK_OBJECT_INVERTED;

	if(flags_)
		*flags_ = flags;

	if(obj)
		data->link->ObjectFlags(data->rc, obj, flags);

	return TRUE;
}

/*************
 * DESCRIPTION:   Read track
 * INPUT:         data     handler data
 *                obj      object
 * OUTPUT:        FALSE if failed
 *************/
static BOOL ReadTrack(HANDLER_DATA *data, OBJECT *obj)
{
	char buffer[256];

	if(data->link->type == LINK_SCENARIO)
	{
		if(!ReadString(data->iff,buffer,256))
			return FALSE;

		return data->link->ObjectTrack(data->rc, obj, buffer);
	}
	return TRUE;
}

/*************
 * DESCRIPTION:   Read name
 * INPUT:         data     handler data
 *                obj      object
 * OUTPUT:        FALSE if failed
 *************/
static BOOL ReadName(HANDLER_DATA *data, OBJECT *obj)
{
	char buffer[256];

	if(data->link->type == LINK_SCENARIO)
	{
		if(!ReadString(data->iff,buffer,256))
			return FALSE;

		return data->link->ObjectName(data->rc, obj, buffer);
	}
	return TRUE;
}

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
 * DESCRIPTION:   reads the object axis from the object file
 * INPUT:         data     handler data
 *                stack    matrix stack
 *                pos      position
 *                orient_x, orient_y, orient_z     orientation
 *                size     size
 * OUTPUT:        FALSE if failed else TRUE
 *************/
static BOOL ReadAxis(HANDLER_DATA *data, MATRIX_STACK *stack, VECTOR *pos,
	VECTOR *orient_x, VECTOR *orient_y, VECTOR *orient_z, VECTOR *size)
{
	MATRIX matrix;
	struct ContextNode *cn;
	long error = 0;
	BOOL done = FALSE;

	while(!error && !done)
	{
		error = ParseIFF(data->iff, IFFPARSE_RAWSTEP);
		if(error < 0 && error != IFFERR_EOC)
		{
			data->errcode = ERR_IFFPARSE;
			return FALSE;
		}

		// Get a pointer to the context node describing the current context
		cn = CurrentChunk(data->iff);
		if(!cn)
			continue;

		if(error == IFFERR_EOC)
		{
			error = 0;
			continue;
		}

		switch (cn->cn_ID)
		{
			case ID_POSI:
				if(!ReadFloat(data->iff,(float*)pos,3))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				break;
			case ID_ALGN:
				if(!ReadFloat(data->iff,(float*)orient_x,3))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				if(!ReadFloat(data->iff,(float*)orient_y,3))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				if(!ReadFloat(data->iff,(float*)orient_z,3))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				break;
			case ID_SIZE:
				if(!ReadFloat(data->iff,(float*)size,3))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				done = TRUE;
				break;
		}
	}

	matrix.SetOTMatrix(orient_x, orient_y, orient_z, pos);

	stack->Push(&matrix);

	matrix = *(stack->GetMatrix());
	SetVector(pos, matrix.m[1], matrix.m[2], matrix.m[3]);
	SetVector(orient_x, matrix.m[5], matrix.m[9], matrix.m[13]);
	SetVector(orient_y, matrix.m[6], matrix.m[10], matrix.m[14]);
	SetVector(orient_z, matrix.m[7], matrix.m[11], matrix.m[15]);

	return TRUE;
}

/*************
 * DESCRIPTION:   read a brush from a RSOB file
 * INPUT:         data     handler data
 *                surf     surface to add brush to
 *                stack    matrix stack
 *                obj      object
 *                actor    actor
 * OUTPUT:        FALSE if failed else TRUE
 *************/
static BOOL ParseBrush(HANDLER_DATA *data, SURFACE *surf, MATRIX_STACK *stack, OBJECT *obj, ACTOR *actor)
{
	BRUSH *brush = NULL;
	BRUSH_OBJECT *brushobj;
	struct ContextNode *cn;
	long error = 0;
	char buffer[256];
	VECTOR pos, ox, oy, oz, size, align;
	ULONG wrap, type, flags, all_flags;

	if(data->rsob_version < 200)
	{
		if(!ReadAxis(data, stack, &pos, &ox, &oy, &oz, &size))
			return FALSE;
	}

	// we need to pop the brush axis, because brushes can't have any child objects
	stack->Pop();

	while(!error)
	{
		error = ParseIFF(data->iff, IFFPARSE_RAWSTEP);
		if(error < 0 && error != IFFERR_EOC)
		{
			data->errcode = ERR_IFFPARSE;
			return FALSE;
		}

		// Get a pointer to the context node describing the current context
		cn = CurrentChunk(data->iff);
		if(!cn)
			continue;

		if(cn->cn_ID == ID_FORM)
			break;

		if(error == IFFERR_EOC)
		{
			error = 0;
			continue;
		}
		switch (cn->cn_ID)
		{
			case ID_NAME:
				if(!ReadString(data->iff,buffer,256))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				brush = data->link->BrushAdd(data->rc, buffer, surf);
				if(!brush)
				{
					data->errcode = ERR_BRUSH;
					return FALSE;
				}
				if(obj)
				{
					brushobj = data->link->BrushObjectAdd(data->rc, obj, actor, data->prevbrush);
					if(!brushobj)
					{
						data->errcode = ERR_BRUSH;
						return FALSE;
					}
					data->link->BrushPos(data->rc, brushobj, &pos);
					data->link->BrushOrient(data->rc, brushobj, &ox, &oy, &oz);
					data->link->BrushSize(data->rc, brushobj, &size);
					if(data->link->type == LINK_RENDERER)
					{
						Orient2Angle(&align, &ox, &oy, &oz);
						data->link->BrushAlignment(data->rc, brushobj, &align);
					}
					data->prevbrush = brushobj;
				}
				break;
			case ID_WRAP:
				if(!ReadULONG(data->iff,&wrap,1))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				break;
			case ID_TYPE:
				if(!ReadULONG(data->iff,&type,1))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				break;
			case ID_FLGS:
				if(!ReadULONG(data->iff,&flags,1))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				break;
		}
	}

	if(brush)
	{
		switch(wrap)
		{
			case BRUSH_WRAP_X:
				all_flags = LINK_BRUSH_WRAPX;
				break;
			case BRUSH_WRAP_Y:
				all_flags = LINK_BRUSH_WRAPY;
				break;
			case BRUSH_WRAP_XY:
				all_flags = LINK_BRUSH_WRAPXY;
				break;
			default:
				all_flags = 0;
				break;
		}

		switch(type)
		{
			case BRUSH_MAP_COLOR:
				all_flags |= LINK_BRUSH_MAP_COLOR;
				break;
			case BRUSH_MAP_REFLECTIVITY:
				all_flags |= LINK_BRUSH_MAP_REFLECTIFITY;
				break;
			case BRUSH_MAP_FILTER:
				all_flags |= LINK_BRUSH_MAP_FILTER;
				break;
			case BRUSH_MAP_ALTITUDE:
				all_flags |= LINK_BRUSH_MAP_ALTITUDE;
				break;
			case BRUSH_MAP_SPECULAR:
				all_flags |= LINK_BRUSH_MAP_SPECULAR;
				break;
		}

		if(flags & BRUSH_SOFT)
			all_flags |= LINK_BRUSH_SOFT;
		if(flags & BRUSH_REPEAT)
			all_flags |= LINK_BRUSH_REPEAT;
		if(flags & BRUSH_MIRROR)
			all_flags |= LINK_BRUSH_MIRROR;

		data->link->BrushFlags(data->rc, brush, all_flags);
	}
	return TRUE;
}

/*************
 * DESCRIPTION:   read a brush object from a RSOB file
 * INPUT:         data     handler data
 *                surf     surface
 *                stack    matrix stack
 *                object   object to add brush object to
 *                actor    actor
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BRUSH_OBJECT *ParseBrushObject(HANDLER_DATA *data, SURFACE *surf, MATRIX_STACK *stack, OBJECT *obj, ACTOR *actor)
{
	VECTOR pos, ox, oy, oz, size, align;
	BRUSH_OBJECT *brushobj;
	struct ContextNode *cn;
	long error = 0;

	if(!ReadAxis(data, stack, &pos, &ox, &oy, &oz, &size))
		return NULL;

	// we need to pop the brush axis, because brushes can't have any child objects
	stack->Pop();

	brushobj = data->link->BrushObjectAdd(data->rc, obj, actor, data->prevbrush);
	if(!brushobj)
	{
		data->errcode = ERR_MEM;
		return NULL;
	}

	while(!error)
	{
		error = ParseIFF(data->iff, IFFPARSE_RAWSTEP);
		if(error < 0 && error != IFFERR_EOC)
		{
			data->errcode = ERR_IFFPARSE;
			return FALSE;
		}

		// Get a pointer to the context node describing the current context
		cn = CurrentChunk(data->iff);
		if(!cn)
			continue;

		if(cn->cn_ID == ID_FORM)
			break;

		if(error == IFFERR_EOC)
		{
			error = 0;
			continue;
		}
		switch (cn->cn_ID)
		{
			case ID_TRCK:
				if(!ReadTrack(data, (OBJECT*)brushobj))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				break;
			case ID_FLGS:
				if(!ReadFlags(data, (OBJECT*)brushobj))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				break;
		}
	}

	data->link->BrushPos(data->rc, brushobj, &pos);
	data->link->BrushOrient(data->rc, brushobj, &ox, &oy, &oz);
	data->link->BrushSize(data->rc, brushobj, &size);
	if(data->link->type == LINK_RENDERER)
	{
		Orient2Angle(&align, &ox, &oy, &oz);
		data->link->BrushAlignment(data->rc, brushobj, &align);
	}
	data->prevbrush = brushobj;

	return brushobj;
}

/*************
 * DESCRIPTION:   parse a raystorm from a RSOB file
 * INPUT:         data     handler data
 *                surf     surface to add texture to
 *                stack    matrix stack
 *                obj      object
 *                actor    actor
 * OUTPUT:        FALSE if failed else TRUE
 *************/
static BOOL ParseRayStormTexture(HANDLER_DATA *data, SURFACE *surf, MATRIX_STACK *stack, OBJECT *obj, ACTOR *actor)
{
	TEXTURE *texture;
	TEXTURE_OBJECT *textureobj;
	struct ContextNode *cn;
	long error = 0;
	char buffer[256];
	VECTOR pos, ox, oy, oz, size;
	UBYTE *tdata;

	if(!ReadAxis(data, stack, &pos, &ox, &oy, &oz, &size))
		return FALSE;

	// we need to pop the texture axis, because textures can't have any child objects
	stack->Pop();

	while(!error)
	{
		error = ParseIFF(data->iff, IFFPARSE_RAWSTEP);
		if(error < 0 && error != IFFERR_EOC)
		{
			data->errcode = ERR_IFFPARSE;
			return FALSE;
		}

		// Get a pointer to the context node describing the current context
		cn = CurrentChunk(data->iff);
		if(!cn)
			continue;

		if(cn->cn_ID == ID_FORM)
			break;

		if(error == IFFERR_EOC)
		{
			error = 0;
			continue;
		}
		switch (cn->cn_ID)
		{
			case ID_NAME:
				if(!ReadString(data->iff,buffer,256))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				texture = data->link->RSTextureAdd(data->rc, buffer, surf);
				if(!texture)
				{
					data->errcode = ERR_TEXTURE;
					return FALSE;
				}
				if(obj)
				{
					textureobj = data->link->TextureObjectAdd(data->rc, obj, actor, data->prevtexture);
					if(!textureobj)
					{
						data->errcode = ERR_TEXTURE;
						return FALSE;
					}
					data->link->TexturePos(data->rc, textureobj, &pos);
					data->link->TextureOrient(data->rc, textureobj, &ox, &oy, &oz);
					data->link->TextureSize(data->rc, textureobj, &size);
					data->prevtexture = textureobj;
				}
				break;
			case ID_PARM:
				tdata = new UBYTE[cn->cn_Size];
				if(!tdata)
				{
					data->errcode = ERR_MEM;
					return FALSE;
				}
				if(!ReadChunkBytes(data->iff, tdata, cn->cn_Size))
				{
					delete tdata;
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				if(!data->link->RSTextureParams(data->rc, texture, cn->cn_Size, tdata))
				{
					delete tdata;
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				delete tdata;
				break;
		}
	}

	return TRUE;
}

/*************
 * DESCRIPTION:   parse a imagine texture from a RSOB file
 * INPUT:         data     handler data
 *                surf     surface to add texture to
 *                stack    matrix stack
 *                obj      object
 *                actor    actor
 * OUTPUT:        FALSE if failed else TRUE
 *************/
static BOOL ParseImagineTexture(HANDLER_DATA *data, SURFACE *surf, MATRIX_STACK *stack, OBJECT *obj, ACTOR *actor)
{
	TEXTURE *texture;
	TEXTURE_OBJECT *textureobj;
	struct ContextNode *cn;
	long error = 0;
	char buffer[256];
	float params[16];
	VECTOR pos, ox, oy, oz, size;
	int i;

	if(!ReadAxis(data, stack, &pos, &ox, &oy, &oz, &size))
		return FALSE;

	// we need to pop the texture axis, because textures can't have any child objects
	stack->Pop();

	while(!error)
	{
		error = ParseIFF(data->iff, IFFPARSE_RAWSTEP);
		if(error < 0 && error != IFFERR_EOC)
		{
			data->errcode = ERR_IFFPARSE;
			return FALSE;
		}

		// Get a pointer to the context node describing the current context
		cn = CurrentChunk(data->iff);
		if(!cn)
			continue;

		if(cn->cn_ID == ID_FORM)
			break;

		if(error == IFFERR_EOC)
		{
			error = 0;
			continue;
		}
		switch (cn->cn_ID)
		{
			case ID_NAME:
				if(!ReadString(data->iff,buffer,256))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				texture = data->link->ITextureAdd(data->rc, buffer, surf);
				if(!texture)
				{
					data->errcode = ERR_TEXTURE;
					return FALSE;
				}
				if(obj)
				{
					textureobj = data->link->TextureObjectAdd(data->rc, obj, actor, data->prevtexture);
					if(!textureobj)
					{
						data->errcode = ERR_TEXTURE;
						return FALSE;
					}
					data->link->TexturePos(data->rc, textureobj, &pos);
					data->link->TextureOrient(data->rc, textureobj, &ox, &oy, &oz);
					data->link->TextureSize(data->rc, textureobj, &size);
					data->prevtexture = textureobj;
				}
				break;
			case ID_PARM:
				if(!ReadFloat(data->iff,params,16))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				for(i=0; i<15; i++)
					data->link->ITextureParams(data->rc, texture, i, params[i]);
				break;
		}
	}

	return TRUE;
}

/*************
 * DESCRIPTION:   read a texture object from a RSOB file
 * INPUT:         data     handler data
 *                surf     surface
 *                stack    matrix stack
 *                object   object to add texture object to
 *                actor    actor
 * OUTPUT:        FALSE if failed else TRUE
 *************/
TEXTURE_OBJECT *ParseTextureObject(HANDLER_DATA *data, SURFACE *surf, MATRIX_STACK *stack, OBJECT *obj, ACTOR *actor)
{
	VECTOR pos, ox, oy, oz, size;
	TEXTURE_OBJECT *textureobj;
	struct ContextNode *cn;
	long error = 0;

	if(!ReadAxis(data, stack, &pos, &ox, &oy, &oz, &size))
		return NULL;

	// we need to pop the texture axis, because textures can't have any child objects
	stack->Pop();

	textureobj = data->link->TextureObjectAdd(data->rc, obj, actor, data->prevtexture);
	if(!textureobj)
	{
		data->errcode = ERR_MEM;
		return NULL;
	}

	while(!error)
	{
		error = ParseIFF(data->iff, IFFPARSE_RAWSTEP);
		if(error < 0 && error != IFFERR_EOC)
		{
			data->errcode = ERR_IFFPARSE;
			return FALSE;
		}

		// Get a pointer to the context node describing the current context
		cn = CurrentChunk(data->iff);
		if(!cn)
			continue;

		if(cn->cn_ID == ID_FORM)
			break;

		if(error == IFFERR_EOC)
		{
			error = 0;
			continue;
		}
		switch (cn->cn_ID)
		{
			case ID_TRCK:
				if(!ReadTrack(data, (OBJECT*)textureobj))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				break;
			case ID_FLGS:
				if(!ReadFlags(data, (OBJECT*)textureobj))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				break;
		}
	}

	data->link->TexturePos(data->rc, textureobj, &pos);
	data->link->TextureOrient(data->rc, textureobj, &ox, &oy, &oz);
	data->link->TextureSize(data->rc, textureobj, &size);

	data->prevtexture = textureobj;

	return textureobj;
}

/*************
 * DESCRIPTION:   read surface from object file
 * INPUT:         data     handler data
 *                stack    matrix stack
 *                obj      object
 *                actor    actor
 * OUTPUT:        created surface
 *************/
static SURFACE *ParseSurface(HANDLER_DATA *data, MATRIX_STACK *stack, OBJECT *obj, ACTOR *actor)
{
	SURFACE *surf;
	struct ContextNode *cn;
	long error = 0;
	char buffer[256];
	COLOR color;
	float num;
	ULONG flags;

	data->prevbrush = NULL;
	data->prevtexture = NULL;

	surf = data->link->SurfaceAdd(data->rc);
	if(!surf)
	{
		data->errcode = ERR_MEM;
		return NULL;
	}
	if(obj)
		data->link->ObjectSurface(data->rc, obj, surf);

	while(!error)
	{
		error = ParseIFF(data->iff, IFFPARSE_RAWSTEP);
		if(error < 0 && error != IFFERR_EOC)
		{
			data->errcode = ERR_IFFPARSE;
			return NULL;
		}

		// Get a pointer to the context node describing the current context
		cn = CurrentChunk(data->iff);
		if(!cn)
			continue;

		if((cn->cn_ID == ID_FORM) && (cn->cn_Type == ID_SURF))
			break;

		if(error == IFFERR_EOC)
		{
			error = 0;
			continue;
		}

		switch (cn->cn_ID)
		{
			case ID_FORM:
				switch(cn->cn_Type)
				{
					case ID_BRSH:
						if(!ParseBrush(data,surf,stack,obj,actor))
							return NULL;
						break;
					case ID_ITXT:
						if(!ParseImagineTexture(data,surf,stack,obj,actor))
							return NULL;
						break;
					case ID_RTXT:
						if(!ParseRayStormTexture(data,surf,stack,obj,actor))
							return NULL;
						break;
				}
				break;
			case ID_NAME:
				if(!ReadString(data->iff,buffer,256))
				{
					data->errcode = ERR_READFILE;
					return NULL;
				}
				if(!data->link->SurfaceName(data->rc, surf, buffer))
				{
					data->errcode = ERR_MEM;
					return NULL;
				}
				break;
			case ID_FLGS:
				if(!ReadULONG(data->iff,&flags,1))
				{
					data->errcode = ERR_READFILE;
					return NULL;
				}
				data->link->SurfaceSetFlags(data->rc, surf, (UWORD)flags);
				break;
			case ID_AMBT:
				if(!ReadFloat(data->iff,(float*)&color,3))
				{
					data->errcode = ERR_READFILE;
					return NULL;
				}
				data->link->SurfaceAmbient(data->rc, surf, color.r, color.g, color.b);
				break;
			case ID_DIFU:
				if(!ReadFloat(data->iff,(float*)&color,3))
				{
					data->errcode = ERR_READFILE;
					return NULL;
				}
				data->link->SurfaceDiffuse(data->rc, surf, color.r, color.g, color.b);
				break;
			case ID_SPEC:
				if(!ReadFloat(data->iff,(float*)&color,3))
				{
					data->errcode = ERR_READFILE;
					return NULL;
				}
				data->link->SurfaceSpecular(data->rc, surf, color.r, color.g, color.b);
				break;
			case ID_REFL:
				if(!ReadFloat(data->iff,(float*)&color,3))
				{
					data->errcode = ERR_READFILE;
					return NULL;
				}
				data->link->SurfaceReflect(data->rc, surf, color.r, color.g, color.b);
				break;
			case ID_TRNS:
				if(!ReadFloat(data->iff,(float*)&color,3))
				{
					data->errcode = ERR_READFILE;
					return NULL;
				}
				data->link->SurfaceTranspar(data->rc, surf, color.r, color.g, color.b);
				break;
			case ID_DIFT:
				if(!ReadFloat(data->iff,(float*)&color,3))
				{
					data->errcode = ERR_READFILE;
					return NULL;
				}
				data->link->SurfaceDiffTrans(data->rc, surf, color.r, color.g, color.b);
				break;
			case ID_SPCT:
				if(!ReadFloat(data->iff,(float*)&color,3))
				{
					data->errcode = ERR_READFILE;
					return NULL;
				}
				data->link->SurfaceSpecTrans(data->rc, surf, color.r, color.g, color.b);
				break;
			case ID_RPHG:
				if(!ReadFloat(data->iff,&num,1))
				{
					data->errcode = ERR_READFILE;
					return NULL;
				}
				data->link->SurfaceRefPhong(data->rc, surf, num);
				break;
			case ID_TPHG:
				if(!ReadFloat(data->iff,&num,1))
				{
					data->errcode = ERR_READFILE;
					return NULL;
				}
				data->link->SurfaceTransPhong(data->rc, surf, num);
				break;
			case ID_FLEN:
				if(!ReadFloat(data->iff,&num,1))
				{
					data->errcode = ERR_READFILE;
					return NULL;
				}
				data->link->SurfaceFoglen(data->rc, surf, num);
				break;
			case ID_IXOR:
				if(!ReadFloat(data->iff,&num,1))
				{
					data->errcode = ERR_READFILE;
					return NULL;
				}
				data->link->SurfaceIndOfRefr(data->rc, surf, num);
				break;
			case ID_TNSL:
				if(!ReadFloat(data->iff,&num,1))
				{
					data->errcode = ERR_READFILE;
					return NULL;
				}
				data->link->SurfaceTransluc(data->rc, surf, num);
				break;
		}
	}

	return surf;
}

/*************
 * DESCRIPTION:   Parse surfaces at the beginning of the file (new for V2)
 * INPUT:         data     handler data
 *                stack    matrix stack
 *                actor    actor
 * OUTPUT:        TRUE -> all done without problems
 *************/
static BOOL ParseSurfaces(HANDLER_DATA *data, MATRIX_STACK *stack, ACTOR *actor)
{
	struct ContextNode *cn;
	long error = 0;

	while(!error)
	{
		error = ParseIFF(data->iff, IFFPARSE_RAWSTEP);
		if(error < 0 && error != IFFERR_EOC)
		{
			data->errcode = ERR_IFFPARSE;
			return FALSE;
		}

		// Get a pointer to the context node describing the current context
		cn = CurrentChunk(data->iff);
		if(!cn)
			continue;

		if((cn->cn_ID == ID_FORM) && (cn->cn_Type == ID_SRFS))
			break;

		if(error == IFFERR_EOC)
		{
			error = 0;
			continue;
		}
		switch (cn->cn_ID)
		{
			case ID_FORM:
				switch(cn->cn_Type)
				{
					case ID_SURF:
						if(!ParseSurface(data,stack,NULL,actor))
							return FALSE;
						break;
				}
				break;
		}
	}

	return TRUE;
}

/*************
 * DESCRIPTION:   read a camera from object file
 * INPUT:         data     handler data
 *                stack    matrix stack
 *                actor    actor
 * OUTPUT:        TRUE -> all done without problems
 *************/
static BOOL ParseCamera(HANDLER_DATA *data, MATRIX_STACK *stack, ACTOR *actor)
{
	CAMERA *camera;
	struct ContextNode *cn;
	long error = 0;
	float num, num1;
	VECTOR pos, ox, oy, oz, size;
	ULONG flags, f;

	if(!ReadAxis(data, stack, &pos, &ox, &oy, &oz, &size))
		return FALSE;

	data->link->ObjectBegin(data->rc);

	camera = data->link->CameraAdd(data->rc);
	if(!camera)
	{
		data->errcode = ERR_READFILE;
		return FALSE;
	}

	data->link->ObjectAxis(data->rc, (OBJECT*)camera, &pos, &ox, &oy, &oz, &size);

	while(!error)
	{
		error = ParseIFF(data->iff, IFFPARSE_RAWSTEP);
		if(error < 0 && error != IFFERR_EOC)
		{
			data->errcode = ERR_IFFPARSE;
			return FALSE;
		}

		// Get a pointer to the context node describing the current context
		cn = CurrentChunk(data->iff);
		if(!cn)
			continue;

		if((cn->cn_ID == ID_FORM) && (cn->cn_Type == ID_CAMR))
			break;

		if(error == IFFERR_EOC)
		{
			error = 0;
			continue;
		}

		switch (cn->cn_ID)
		{
			case ID_NAME:
				if(!ReadName(data, (OBJECT*)camera))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				break;
			case ID_TRCK:
				if(!ReadTrack(data, (OBJECT*)camera))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				break;
			case ID_FLGS:
				if(!ReadFlags(data, (OBJECT*)camera, &flags))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				f = 0;
				if(flags & RSCN_OBJECT_CAMERA_VFOV)
					f |= LINK_CAMERA_VFOV;
				if(flags & RSCN_OBJECT_CAMERA_FOCUSTRACK)
					f |= LINK_CAMERA_FOCUSTRACK;
				if(flags & RSCN_OBJECT_CAMERA_FASTDOF)
					f |= LINK_CAMERA_FASTDOF;
				data->link->CameraFlags(data->rc, camera, f);
				break;
			case ID_FDST:
				if(!ReadFloat(data->iff,&num,1))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				data->link->CameraFocalDist(data->rc, camera, num);
				break;
			case ID_APER:
				if(!ReadFloat(data->iff,&num,1))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				data->link->CameraAperture(data->rc, camera, num);
				break;
			case ID_FDOV:
				if(!ReadFloat(data->iff,&num,1))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				if(!ReadFloat(data->iff,&num1,1))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				data->link->CameraFOV(data->rc, camera, num, num1);
				break;
		}
	}
	return TRUE;
}

/*************
 * DESCRIPTION:   parse the new V2 object surface chunk
 * INPUT:         data     handler data
 *                stack    matrix stack
 *                obj      object the surface is assigned to
 *                actor    actor
 * OUTPUT:        surface
 *************/
static SURFACE *ParseObjectSurface(HANDLER_DATA *data, MATRIX_STACK *stack, OBJECT *obj, ACTOR *actor)
{
	struct ContextNode *cn;
	long error = 0;
	char buffer[256];
	SURFACE *surf;

	data->prevbrush = NULL;
	data->prevtexture = NULL;

	while(!error)
	{
		error = ParseIFF(data->iff, IFFPARSE_RAWSTEP);
		if(error < 0 && error != IFFERR_EOC)
		{
			data->errcode = ERR_IFFPARSE;
			return NULL;
		}

		// Get a pointer to the context node describing the current context
		cn = CurrentChunk(data->iff);
		if(!cn)
			continue;

		if((cn->cn_ID == ID_FORM) && (cn->cn_Type == ID_SURF))
			break;

		if(error == IFFERR_EOC)
		{
			error = 0;
			continue;
		}
		switch (cn->cn_ID)
		{
			case ID_FORM:
				switch(cn->cn_Type)
				{
					case ID_BRSH:
						if(!ParseBrushObject(data, surf, stack, obj, actor))
							return FALSE;
						break;
					case ID_ITXT:
					case ID_RTXT:
						if(!ParseTextureObject(data, surf, stack, obj, actor))
							return FALSE;
						break;
				}
				break;
			case ID_NAME:
				if(!ReadString(data->iff,buffer,256))
				{
					data->errcode = ERR_READFILE;
					return NULL;
				}
				surf = data->link->SurfaceGetByName(data->rc, buffer);
				if(obj)
					data->link->ObjectSurface(data->rc, obj, surf);
				break;
		}
	}

	return surf;
}

/*************
 * DESCRIPTION:   parse a sphere from object file
 * INPUT:         data     handler data
 *                stack    matrix stack
 *                actor    actor
 * OUTPUT:        TRUE -> all done without problems
 *************/
static BOOL ParseSphere(HANDLER_DATA *data, MATRIX_STACK *stack, ACTOR *actor)
{
	SURFACE *surf;
	struct ContextNode *cn;
	long error = 0;
	VECTOR pos, ox, oy, oz, size;
	OBJECT *obj;

	if(!ReadAxis(data, stack, &pos, &ox, &oy, &oz, &size))
		return FALSE;

	data->link->ObjectBegin(data->rc);

	obj = (OBJECT*)data->link->SphereAdd(data->rc);
	if(!obj)
	{
		data->errcode = ERR_MEM;
		return FALSE;
	}

	while(!error)
	{
		error = ParseIFF(data->iff, IFFPARSE_RAWSTEP);
		if(error < 0 && error != IFFERR_EOC)
		{
			data->errcode = ERR_IFFPARSE;
			return FALSE;
		}

		// Get a pointer to the context node describing the current context
		cn = CurrentChunk(data->iff);
		if(!cn)
			continue;

		if((cn->cn_ID == ID_FORM) && (cn->cn_Type == ID_SPHR))
			break;

		if(error == IFFERR_EOC)
		{
			error = 0;
			continue;
		}
		switch (cn->cn_ID)
		{
			case ID_FORM:
				switch(cn->cn_Type)
				{
					case ID_SURF:
						surf = HandleSurface(data, stack, obj, actor);
						if(!surf)
							return FALSE;
						break;
				}
				break;
			case ID_NAME:
				if(!ReadName(data, obj))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				break;
			case ID_TRCK:
				if(!ReadTrack(data, obj))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				break;
			case ID_FLGS:
				if(!ReadFlags(data, obj))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				break;
			case ID_RADI:
				if(!ReadFloat(data->iff,&size.x,1))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				break;
		}
	}

	if(data->link->type == LINK_SCENARIO)
	{
		data->link->ObjectAxis(data->rc, obj, &pos, &ox, &oy, &oz, &size);
	}
	else
	{
		data->link->ObjectActor(data->rc, obj, actor);
		data->link->SphereAxis(data->rc, obj, &pos, size.x);
	}

	return TRUE;
}

/*************
 * DESCRIPTION:   parse a box from object file
 * INPUT:         data     handler data
 *                stack    matrix stack
 *                actor    actor
 * OUTPUT:        TRUE -> all done without problems
 *************/
static BOOL ParseBox(HANDLER_DATA *data, MATRIX_STACK *stack, ACTOR *actor)
{
	SURFACE *surf;
	struct ContextNode *cn;
	long error = 0;
	VECTOR pos, ox, oy, oz, size;
	VECTOR lowbounds, highbounds;
	OBJECT *obj;

	data->link->ObjectBegin(data->rc);

	obj = data->link->BoxAdd(data->rc);
	if(!obj)
	{
		data->errcode = ERR_MEM;
		return FALSE;
	}

	if(!ReadAxis(data, stack, &pos, &ox, &oy, &oz, &size))
		return FALSE;

	while(!error)
	{
		error = ParseIFF(data->iff, IFFPARSE_RAWSTEP);
		if(error < 0 && error != IFFERR_EOC)
		{
			data->errcode = ERR_IFFPARSE;
			return FALSE;
		}

		// Get a pointer to the context node describing the current context
		cn = CurrentChunk(data->iff);
		if(!cn)
			continue;

		if((cn->cn_ID == ID_FORM) && (cn->cn_Type == ID_BOX))
			break;

		if(error == IFFERR_EOC)
		{
			error = 0;
			continue;
		}
		switch (cn->cn_ID)
		{
			case ID_FORM:
				switch(cn->cn_Type)
				{
					case ID_SURF:
						surf = HandleSurface(data, stack, obj, actor);
						if(!surf)
							return FALSE;
						break;
				}
				break;
			case ID_NAME:
				if(!ReadName(data, obj))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				break;
			case ID_TRCK:
				if(!ReadTrack(data, obj))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				break;
			case ID_FLGS:
				if(!ReadFlags(data, obj))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				break;
			case ID_LBND:
				if(!ReadFloat(data->iff,(float*)&lowbounds,3))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				break;
			case ID_HBND:
				if(!ReadFloat(data->iff,(float*)&highbounds,3))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				break;
		}
	}

	data->link->BoxBounds(data->rc, obj, &lowbounds, &highbounds);

	if(data->link->type == LINK_SCENARIO)
	{
		data->link->ObjectAxis(data->rc, obj, &pos, &ox, &oy, &oz, &size);
	}
	else
	{
		data->link->ObjectActor(data->rc, obj, actor);
		data->link->BoxAxis(data->rc, obj, &pos, &ox, &oy, &oz);
	}

	return TRUE;
}

/*************
 * DESCRIPTION:   parse a cylinder from object file
 * INPUT:         data     handler data
 *                stack    matrix stack
 *                actor    actor
 * OUTPUT:        TRUE -> all done without problems
 *************/
static BOOL ParseCylinder(HANDLER_DATA *data, MATRIX_STACK *stack, ACTOR *actor)
{
	SURFACE *surf;
	struct ContextNode *cn;
	long error = 0;
	VECTOR pos, ox, oy, oz, size;
	OBJECT *obj;
	ULONG flags, f;

	data->link->ObjectBegin(data->rc);

	obj = data->link->CylinderAdd(data->rc);
	if(!obj)
	{
		data->errcode = ERR_MEM;
		return FALSE;
	}

	if(!ReadAxis(data, stack, &pos, &ox, &oy, &oz, &size))
		return FALSE;

	while(!error)
	{
		error = ParseIFF(data->iff, IFFPARSE_RAWSTEP);
		if(error < 0 && error != IFFERR_EOC)
		{
			data->errcode = ERR_IFFPARSE;
			return FALSE;
		}

		// Get a pointer to the context node describing the current context
		cn = CurrentChunk(data->iff);
		if(!cn)
			continue;

		if((cn->cn_ID == ID_FORM) && (cn->cn_Type == ID_CYLR))
			break;

		if(error == IFFERR_EOC)
		{
			error = 0;
			continue;
		}
		switch (cn->cn_ID)
		{
			case ID_FORM:
				switch(cn->cn_Type)
				{
					case ID_SURF:
						surf = HandleSurface(data, stack, obj, actor);
						if(!surf)
							return FALSE;
						break;
				}
				break;
			case ID_NAME:
				if(!ReadName(data, obj))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				break;
			case ID_TRCK:
				if(!ReadTrack(data, obj))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				break;
			case ID_FLGS:
				if(!ReadFlags(data, obj, &flags))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				f = 0;
				if(flags & RSCN_OBJECT_OPENTOP)
					f |= LINK_CYLINDER_OPENTOP;
				if(flags & RSCN_OBJECT_OPENBOTTOM)
					f |= LINK_CYLINDER_OPENBOTTOM;
				data->link->CylinderFlags(data->rc, obj, f);
				break;
		}
	}

	if(data->link->type == LINK_SCENARIO)
	{
		data->link->ObjectAxis(data->rc, obj, &pos, &ox, &oy, &oz, &size);
	}
	else
	{
		data->link->ObjectActor(data->rc, obj, actor);
		data->link->CylinderAxis(data->rc, obj, &pos, &ox, &oy, &oz, &size);
	}

	return TRUE;
}

/*************
 * DESCRIPTION:   parse a cone from object file
 * INPUT:         data     handler data
 *                stack    matrix stack
 *                actor    actor
 * OUTPUT:        TRUE -> all done without problems
 *************/
static BOOL ParseCone(HANDLER_DATA *data, MATRIX_STACK *stack, ACTOR *actor)
{
	SURFACE *surf;
	struct ContextNode *cn;
	long error = 0;
	VECTOR pos, ox, oy, oz, size;
	OBJECT *obj;
	ULONG flags, f;

	data->link->ObjectBegin(data->rc);

	obj = data->link->ConeAdd(data->rc);
	if(!obj)
	{
		data->errcode = ERR_MEM;
		return FALSE;
	}

	if(!ReadAxis(data, stack, &pos, &ox, &oy, &oz, &size))
		return FALSE;

	while(!error)
	{
		error = ParseIFF(data->iff, IFFPARSE_RAWSTEP);
		if(error < 0 && error != IFFERR_EOC)
		{
			data->errcode = ERR_IFFPARSE;
			return FALSE;
		}

		// Get a pointer to the context node describing the current context
		cn = CurrentChunk(data->iff);
		if(!cn)
			continue;

		if((cn->cn_ID == ID_FORM) && (cn->cn_Type == ID_CONE))
			break;

		if(error == IFFERR_EOC)
		{
			error = 0;
			continue;
		}
		switch (cn->cn_ID)
		{
			case ID_FORM:
				switch(cn->cn_Type)
				{
					case ID_SURF:
						surf = HandleSurface(data, stack, obj, actor);
						if(!surf)
							return FALSE;
						break;
				}
				break;
			case ID_NAME:
				if(!ReadName(data, obj))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				break;
			case ID_TRCK:
				if(!ReadTrack(data, obj))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				break;
			case ID_FLGS:
				if(!ReadFlags(data, obj, &flags))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				f = 0;
				if(flags & RSCN_OBJECT_OPENBOTTOM)
					f |= LINK_CONE_OPENBOTTOM;
				data->link->ConeFlags(data->rc, obj, f);
				break;
		}
	}

	if(data->link->type == LINK_SCENARIO)
	{
		data->link->ObjectAxis(data->rc, obj, &pos, &ox, &oy, &oz, &size);
	}
	else
	{
		data->link->ObjectActor(data->rc, obj, actor);
		data->link->ConeAxis(data->rc, obj, &pos, &ox, &oy, &oz, &size);
	}

	return TRUE;
}

/*************
 * DESCRIPTION:   parse a plane from object file
 * INPUT:         data     handler data
 *                stack    matrix stack
 *                actor    actor
 * OUTPUT:        TRUE -> all done without problems
 *************/
static BOOL ParsePlane(HANDLER_DATA *data, MATRIX_STACK *stack, ACTOR *actor)
{
	struct ContextNode *cn;
	long error = 0;
	SURFACE *surf;
	VECTOR pos, ox, oy, oz, size;
	OBJECT *obj;

	data->link->ObjectBegin(data->rc);

	obj = data->link->PlaneAdd(data->rc);
	if(!obj)
	{
		data->errcode = ERR_MEM;
		return FALSE;
	}

	if(!ReadAxis(data, stack, &pos, &ox, &oy, &oz, &size))
		return FALSE;

	while(!error)
	{
		error = ParseIFF(data->iff, IFFPARSE_RAWSTEP);
		if(error < 0 && error != IFFERR_EOC)
		{
			data->errcode = ERR_IFFPARSE;
			return FALSE;
		}

		// Get a pointer to the context node describing the current context
		cn = CurrentChunk(data->iff);
		if(!cn)
			continue;

		if((cn->cn_ID == ID_FORM) && (cn->cn_Type == ID_PLAN))
			break;

		if(error == IFFERR_EOC)
		{
			error = 0;
			continue;
		}
		switch (cn->cn_ID)
		{
			case ID_FORM:
				switch(cn->cn_Type)
				{
					case ID_SURF:
						surf = HandleSurface(data, stack, obj, actor);
						if(!surf)
							return FALSE;
						break;
				}
				break;
			case ID_NAME:
				if(!ReadName(data, obj))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				break;
			case ID_TRCK:
				if(!ReadTrack(data, obj))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				break;
			case ID_FLGS:
				if(!ReadFlags(data, obj))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				break;
		}
	}

	if(data->link->type == LINK_SCENARIO)
	{
		data->link->ObjectAxis(data->rc, obj, &pos, &ox, &oy, &oz, &size);
	}
	else
	{
		data->link->ObjectActor(data->rc, obj, actor);
		data->link->PlaneAxis(data->rc, obj, &pos, &oy);
	}

	return TRUE;
}

/*************
 * DESCRIPTION:   parse a CSG object from object file
 * INPUT:         data     handler data
 *                stack    matrix stack
 *                actor    actor
 * OUTPUT:        TRUE -> all done without problems
 *************/
static BOOL ParseCSG(HANDLER_DATA *data, MATRIX_STACK *stack, ACTOR *actor)
{
	struct ContextNode *cn;
	long error = 0;
	VECTOR pos, ox, oy, oz, size;
	OBJECT *obj;
	UWORD operation;

	data->link->ObjectBegin(data->rc);

	obj = data->link->CSGAdd(data->rc);
	if(!obj)
	{
		data->errcode = ERR_MEM;
		return FALSE;
	}

	if(!ReadAxis(data, stack, &pos, &ox, &oy, &oz, &size))
		return FALSE;

	while(!error)
	{
		error = ParseIFF(data->iff, IFFPARSE_RAWSTEP);
		if(error < 0 && error != IFFERR_EOC)
		{
			data->errcode = ERR_IFFPARSE;
			return FALSE;
		}

		// Get a pointer to the context node describing the current context
		cn = CurrentChunk(data->iff);
		if(!cn)
			continue;

		if((cn->cn_ID == ID_FORM) && (cn->cn_Type == ID_CSG))
			break;

		if(error == IFFERR_EOC)
		{
			error = 0;
			continue;
		}
		switch (cn->cn_ID)
		{
			case ID_NAME:
				if(!ReadName(data, obj))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				break;
			case ID_TRCK:
				if(!ReadTrack(data, obj))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				break;
			case ID_FLGS:
				if(!ReadFlags(data, obj))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				break;
			case ID_OPER:
				if(!ReadWord(data->iff, (WORD*)&operation, 1))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				switch(operation)
				{
					case RSCN_CSG_UNION:
						operation = LINK_CSG_UNION;
						break;
					case RSCN_CSG_INTERSECTION:
						operation = LINK_CSG_INTERSECTION;
						break;
					case RSCN_CSG_DIFFERENCE:
						operation = LINK_CSG_DIFFERENCE;
						break;
				}
				data->link->CSGOperation(data->rc, obj, operation);
				break;
		}
	}

	if(data->link->type == LINK_SCENARIO)
	{
		data->link->ObjectAxis(data->rc, obj, &pos, &ox, &oy, &oz, &size);
	}
	else
	{
		data->link->ObjectActor(data->rc, obj, actor);
	}

	return TRUE;
}

/*************
 * DESCRIPTION:   parse a flare from a RSOB file
 * INPUT:         data     handler data
 *                light    light to add flare to
 * OUTPUT:        TRUE -> all done without problems
 *************/
static BOOL ParseFlare(HANDLER_DATA *data, LIGHT *light)
{
	FLARE *flare;
	struct ContextNode *cn;
	long error = 0;
	COLOR color;
	ULONG lnum;
	float num;

	flare = data->link->FlareAdd(data->rc, light);
	if(!flare)
	{
		data->errcode = ERR_MEM;
		return FALSE;
	}

	while(!error)
	{
		error = ParseIFF(data->iff, IFFPARSE_RAWSTEP);
		if(error < 0 && error != IFFERR_EOC)
		{
			data->errcode = ERR_IFFPARSE;
			return FALSE;
		}

		// Get a pointer to the context node describing the current context
		cn = CurrentChunk(data->iff);
		if(!cn)
			continue;

		if((cn->cn_ID == ID_FORM) && (cn->cn_Type == ID_FLAR))
			break;

		if(error == IFFERR_EOC)
		{
			error = 0;
			continue;
		}
		switch (cn->cn_ID)
		{
			case ID_COLR:
				if(!ReadFloat(data->iff,(float*)&color,3))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				data->link->FlareColor(data->rc, flare, color.r, color.g, color.b);
				break;
			case ID_POSI:
				if(!ReadFloat(data->iff,&num,1))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				data->link->FlarePosition(data->rc, flare, num);
				break;
			case ID_RADI:
				if(!ReadFloat(data->iff,&num,1))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				data->link->FlareRadius(data->rc, flare, num);
				break;
			case ID_TYPE:
				if(!ReadULONG(data->iff,&lnum,1))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				data->link->FlareType(data->rc, flare, lnum);
				break;
			case ID_FUNC:
				if(!ReadULONG(data->iff,&lnum,1))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				data->link->FlareFunction(data->rc, flare, lnum);
				break;
			case ID_EDGE:
				if(!ReadULONG(data->iff,&lnum,1))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				data->link->FlareEdges(data->rc, flare, lnum);
				break;
			case ID_TILT:
				if(!ReadFloat(data->iff,&num,1))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				data->link->FlareTilt(data->rc, flare, num);
				break;
		}
	}

	return TRUE;
}

/*************
 * DESCRIPTION:   parse a light star from a RSOB file
 * INPUT:         data     handler data
 *                light    light to add star to
 * OUTPUT:        TRUE -> all done without problems
 *************/
static BOOL ParseStar(HANDLER_DATA *data, LIGHT *light)
{
	STAR *star;
	struct ContextNode *cn;
	long error = 0;
	ULONG flags, lnum, spikes = 0;
	float num, *inte;

	star = data->link->StarAdd(data->rc, light);
	if(!star)
	{
		data->errcode = ERR_MEM;
		return FALSE;
	}

	while(!error)
	{
		error = ParseIFF(data->iff, IFFPARSE_RAWSTEP);
		if(error < 0 && error != IFFERR_EOC)
		{
			data->errcode = ERR_IFFPARSE;
			return FALSE;
		}

		// Get a pointer to the context node describing the current context
		cn = CurrentChunk(data->iff);
		if(!cn)
			continue;

		if((cn->cn_ID == ID_FORM) && (cn->cn_Type == ID_STAR))
			break;

		if(error == IFFERR_EOC)
		{
			error = 0;
			continue;
		}
		switch (cn->cn_ID)
		{
			case ID_FLGS:
				if(!ReadULONG(data->iff,&lnum,1))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				flags = 0;
				if(lnum & RSCN_STAR_ENABLE)
					flags |= LINK_STAR_ENABLE;
				if(lnum & RSCN_STAR_HALOENABLE)
					flags |= LINK_STAR_HALOENABLE;
				if(lnum & RSCN_STAR_RANDOM)
					flags |= LINK_STAR_RANDOM;
				data->link->StarFlags(data->rc, star, flags);
				break;
			case ID_SRAD:
				if(!ReadFloat(data->iff,&num,1))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				data->link->StarRadius(data->rc, star, num);
				break;
			case ID_TILT:
				if(!ReadFloat(data->iff,&num,1))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				data->link->StarTilt(data->rc, star, num);
				break;
			case ID_SPIK:
				if(!ReadULONG(data->iff,&spikes,1))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				data->link->StarSpikes(data->rc, star, spikes);
				break;
			case ID_HRAD:
				if(!ReadFloat(data->iff,&num,1))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				data->link->StarHaloradius(data->rc, star, num);
				break;
			case ID_IHRD:
				if(!ReadFloat(data->iff,&num,1))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				data->link->StarInnerHaloradius(data->rc, star, num);
				break;
			case ID_RANG:
				if(!ReadFloat(data->iff,&num,1))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				data->link->StarRange(data->rc, star, num);
				break;
			case ID_INTE:
				inte = new float[spikes];
				if(!inte)
				{
					data->errcode = ERR_MEM;
					return FALSE;
				}
				if(!ReadFloat(data->iff,inte,spikes))
				{
					delete inte;
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				if(!data->link->StarIntensities(data->rc, star, spikes, inte))
				{
					delete inte;
					data->errcode = ERR_MEM;
					return FALSE;
				}
				delete inte;
				break;
		}
	}

	return TRUE;
}

/*************
 * DESCRIPTION:   parse a light source from a RSOB file (can hanlde all different light types)
 * INPUT:         data     handler data
 *                id       chunk id
 *                stack    matrix stack
 *                actor    actor
 * OUTPUT:        TRUE -> all done without problems
 *************/
static BOOL ParseLight(HANDLER_DATA *data, ULONG id, MATRIX_STACK *stack, ACTOR *actor)
{
	LIGHT *light;
	struct ContextNode *cn;
	long error = 0;
	VECTOR pos, ox, oy, oz, size;
	COLOR color;
	ULONG lnum;
	float num;
	ULONG flags, f;

	if(!ReadAxis(data, stack, &pos, &ox, &oy, &oz, &size))
		return FALSE;

	data->link->ObjectBegin(data->rc);

	switch(id)
	{
		case ID_PLGT:
			light = data->link->PointLightAdd(data->rc);
			break;
		case ID_SLGT:
			light = data->link->SpotLightAdd(data->rc);
			break;
		case ID_DLGT:
			light = data->link->DirectionalLightAdd(data->rc);
			break;
	}
	if(!light)
	{
		data->errcode = ERR_MEM;
		return FALSE;
	}

	if(data->link->type == LINK_SCENARIO)
		data->link->ObjectAxis(data->rc, (OBJECT*)light, &pos, &ox, &oy, &oz, &size);
	else
		data->link->LightPos(data->rc, light, &pos);

	while(!error)
	{
		error = ParseIFF(data->iff, IFFPARSE_RAWSTEP);
		if(error < 0 && error != IFFERR_EOC)
		{
			data->errcode = ERR_IFFPARSE;
			return FALSE;
		}

		// Get a pointer to the context node describing the current context
		cn = CurrentChunk(data->iff);
		if(!cn)
			continue;

		if((cn->cn_ID == ID_FORM) && (cn->cn_Type == id))
			break;

		if(error == IFFERR_EOC)
		{
			error = 0;
			continue;
		}
		switch (cn->cn_ID)
		{
			case ID_FORM:
				switch(cn->cn_Type)
				{
					case ID_FLAR:
						if(!ParseFlare(data,(LIGHT*)light))
							return NULL;
						break;
					case ID_STAR:
						if(!ParseStar(data,(LIGHT*)light))
							return NULL;
						break;
				}
				break;
			case ID_NAME:
				if(!ReadName(data, (OBJECT*)light))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				break;
			case ID_TRCK:
				if(!ReadTrack(data, (OBJECT*)light))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				break;
			case ID_FLGS:
				if(!ReadFlags(data, (OBJECT*)light, &flags))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				f = 0;
				if(flags & RSCN_OBJECT_LIGHT_SHADOW)
					f |= LINK_LIGHT_SHADOW;
				if(flags & RSCN_OBJECT_LIGHT_FLARES)
					f |= LINK_LIGHT_FLARES;
				if(flags & RSCN_OBJECT_LIGHT_STAR)
					f |= LINK_LIGHT_STAR;
				if(flags & RSCN_OBJECT_LIGHT_SHADOWMAP)
					f |= LINK_LIGHT_SHADOWMAP;
				if(flags & RSCN_OBJECT_LIGHT_TRACKFALLOFF)
					f |= LINK_LIGHT_TRACKFALLOFF;
				data->link->LightFlags(data->rc, light, f);
				break;
			case ID_COLR:
				if(!ReadFloat(data->iff,(float*)&color,3))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				data->link->LightColor(data->rc, light, color.r, color.g, color.b);
				break;
			case ID_DIST:
				if(!ReadFloat(data->iff,&num,1))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				data->link->LightFallOff(data->rc, light, num);
				break;
			case ID_RADI:
				if(!ReadFloat(data->iff,&num,1))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				data->link->LightRadius(data->rc, light, num);
				break;
			case ID_SMSZ:
				if(!ReadULONG(data->iff,&lnum,1))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				data->link->LightShadowMapSize(data->rc, light, lnum);
				break;
			case ID_ANGL:
				if(id == ID_SLGT)
				{
					if(!ReadFloat(data->iff,&num,1))
					{
						data->errcode = ERR_READFILE;
						return FALSE;
					}
					data->link->SpotLightAngle(data->rc, light, num);
				}
				break;
			case ID_FALL:
				if(id == ID_SLGT)
				{
					if(!ReadFloat(data->iff, &num, 1))
					{
						data->errcode = ERR_READFILE;
						return FALSE;
					}
					data->link->SpotLightFallOffRadius(data->rc, light, num);
				}
				break;
		}
	}

	return TRUE;
}

/*************
 * DESCRIPTION:   parse a mesh from a RSCN file
 * INPUT:         data     handler data
 *                stack    matrix stack
 *                actor    actor
 *                trans, orient_x, orient_y, orient_z, scale
 *                         transformations for object
 * OUTPUT:        TRUE -> all done without problems
 *************/
static BOOL ParseMesh(HANDLER_DATA *data, MATRIX_STACK *stack, ACTOR *actor,
	VECTOR *trans, VECTOR *orient_x, VECTOR *orient_y, VECTOR *orient_z, VECTOR *scale)
{
	SURFACE *surf;
	struct ContextNode *cn;
	long error = 0;
	VECTOR pos, ox, oy, oz, size;
	int pointcount, edgecount, triacount=0, i;
	ULONG flags=0;
	VECTOR *points = NULL;
	EDGE *edges = NULL;
	TRIANGLE *trias = NULL;
	OBJECT *obj;

	if(!ReadAxis(data, stack, &pos, &ox, &oy, &oz, &size))
		return FALSE;

	if(data->link->type == LINK_SCENARIO)
	{
		obj = data->link->MeshCreate(data->rc);
		if(!obj)
		{
			data->errcode = ERR_MEM;
			return FALSE;
		}
	}

	while(!error)
	{
		error = ParseIFF(data->iff, IFFPARSE_RAWSTEP);
		if(error < 0 && error != IFFERR_EOC)
		{
			data->errcode = ERR_IFFPARSE;
			if(points)
				delete points;
			if(edges)
				delete edges;
			if(trias)
				delete trias;
			return FALSE;
		}

		// Get a pointer to the context node describing the current context
		cn = CurrentChunk(data->iff);
		if(!cn)
			continue;

		if((cn->cn_ID == ID_FORM) && (cn->cn_Type == ID_MESH))
			break;

		if(error == IFFERR_EOC)
		{
			error = 0;
			continue;
		}
		switch (cn->cn_ID)
		{
			case ID_NAME:
				if(!ReadName(data, obj))
				{
					data->errcode = ERR_READFILE;
					if(points)
						delete points;
					if(edges)
						delete edges;
					if(trias)
						delete trias;
					return FALSE;
				}
				break;
			case ID_TRCK:
				if(!ReadTrack(data, obj))
				{
					data->errcode = ERR_READFILE;
					if(points)
						delete points;
					if(edges)
						delete edges;
					if(trias)
						delete trias;
					return FALSE;
				}
				break;
			case ID_FLGS:
				if(!ReadFlags(data, obj, &flags))
				{
					data->errcode = ERR_READFILE;
					if(points)
						delete points;
					if(edges)
						delete edges;
					if(trias)
						delete trias;
					return FALSE;
				}
				break;
			case ID_PNTS:
				pointcount = cn->cn_Size/12;
				points = new VECTOR[pointcount];
				if(!points)
				{
					data->errcode = ERR_MEM;
					if(edges)
						delete edges;
					if(trias)
						delete trias;
					return FALSE;
				}
				if(!ReadFloat(data->iff, (float*)(points), cn->cn_Size>>2))
				{
					data->errcode = ERR_READFILE;
					if(points)
						delete points;
					if(edges)
						delete edges;
					if(trias)
						delete trias;
					return FALSE;
				}
				break;
			case ID_EDGE:
				edgecount = cn->cn_Size/6;
				edges = new EDGE[edgecount];
				if(!edges)
				{
					data->errcode = ERR_MEM;
					if(points)
						delete points;
					if(trias)
						delete trias;
					return FALSE;
				}
				if(!ReadWord(data->iff, (WORD*)(edges), cn->cn_Size>>1))
				{
					data->errcode = ERR_READFILE;
					if(points)
						delete points;
					if(edges)
						delete edges;
					if(trias)
						delete trias;
					return FALSE;
				}
				break;
			case ID_FACE:
				triacount = cn->cn_Size/6;
				trias = new TRIANGLE[triacount];
				if(!trias)
				{
					data->errcode = ERR_MEM;
					if(points)
						delete points;
					if(edges)
						delete edges;
					return FALSE;
				}
				if(!ReadWord(data->iff, (WORD*)(trias), cn->cn_Size>>1))
				{
					data->errcode = ERR_READFILE;
					if(points)
						delete points;
					if(edges)
						delete edges;
					if(trias)
						delete trias;
					return FALSE;
				}
				break;
			case ID_FORM:
				switch(cn->cn_Type)
				{
					case ID_SURF:
						surf = HandleSurface(data, stack, obj, actor);
						if(!surf)
							return FALSE;
						break;
				}
				break;
		}
	}

	if(triacount)
	{
		for(i=0; i<pointcount; i++)
			stack->MultVectStack(&points[i]);

		if(data->link->type == LINK_SCENARIO)
		{
			data->link->ObjectAxis(data->rc, obj, &pos, &ox, &oy, &oz, &size);

			if(!data->link->MeshAddScenario(data->rc, obj,
				points, pointcount,
				(LINK_EDGE*)edges, edgecount,
				(LINK_MESH*)trias, triacount,
				&data->scale,
				flags & RSCN_OBJECT_MESH_NOPHONG))
			{
				data->errcode = ERR_MEM;
				if(points)
					delete points;
				if(edges)
					delete edges;
				if(trias)
					delete trias;
				return FALSE;
			}
		}
		else
		{
			if(!data->link->MeshAddRenderer(data->rc, surf, NULL, actor,
				points, pointcount,
				(LINK_EDGE*)edges, edgecount,
				(LINK_MESH*)trias, triacount,
				trans, orient_x, orient_y, orient_z, scale,
				flags & RSCN_OBJECT_MESH_NOPHONG))
			{
				data->errcode = ERR_MEM;
				if(points)
					delete points;
				if(edges)
					delete edges;
				if(trias)
					delete trias;
				return FALSE;
			}
		}
	}
	else
	{
		if(data->link->type == LINK_SCENARIO)
		{
			data->link->ObjectAxis(data->rc, obj, &pos, &ox, &oy, &oz, &size);
		}
	}

	if(points)
		delete points;
	if(edges)
		delete edges;
	if(trias)
		delete trias;

	return TRUE;
}

/*************
 * DESCRIPTION:   parse a SOR from object file
 * INPUT:         data     handler data
 *                stack    matrix stack
 *                actor    actor
 * OUTPUT:        TRUE -> all done without problems
 *************/
static BOOL ParseSOR(HANDLER_DATA *data, MATRIX_STACK *stack, ACTOR *actor)
{
	SURFACE *surf;
	struct ContextNode *cn;
	long error = 0;
	VECTOR pos, ox, oy, oz, size;
	OBJECT *obj;
	ULONG flags, f;
	VECT2D *points;

	data->link->ObjectBegin(data->rc);

	obj = data->link->SORAdd(data->rc);
	if(!obj)
	{
		data->errcode = ERR_MEM;
		return FALSE;
	}

	if(!ReadAxis(data, stack, &pos, &ox, &oy, &oz, &size))
		return FALSE;

	while(!error)
	{
		error = ParseIFF(data->iff, IFFPARSE_RAWSTEP);
		if(error < 0 && error != IFFERR_EOC)
		{
			data->errcode = ERR_IFFPARSE;
			return FALSE;
		}

		// Get a pointer to the context node describing the current context
		cn = CurrentChunk(data->iff);
		if(!cn)
			continue;

		if((cn->cn_ID == ID_FORM) && (cn->cn_Type == ID_SOR))
			break;

		if(error == IFFERR_EOC)
		{
			error = 0;
			continue;
		}
		switch (cn->cn_ID)
		{
			case ID_FORM:
				switch(cn->cn_Type)
				{
					case ID_SURF:
						surf = HandleSurface(data, stack, obj, actor);
						if(!surf)
							return FALSE;
						break;
				}
				break;
			case ID_NAME:
				if(!ReadName(data, obj))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				break;
			case ID_TRCK:
				if(!ReadTrack(data, obj))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				break;
			case ID_FLGS:
				if(!ReadFlags(data, obj, &flags))
				{
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				f = 0;
				if(flags & RSCN_OBJECT_OPENTOP)
					f |= LINK_SOR_OPENTOP;
				if(flags & RSCN_OBJECT_OPENBOTTOM)
					f |= LINK_SOR_OPENBOTTOM;
				if(flags & RSCN_OBJECT_SOR_ACCURATE)
					f |= LINK_SOR_ACCURATE;
				data->link->SORFlags(data->rc, obj, f);
				break;
			case ID_PNTS:
				points = new VECT2D[cn->cn_Size>>3];
				if(!points)
				{
					data->errcode = ERR_MEM;
					return FALSE;
				}
				if(!ReadFloat(data->iff, (float*)points, cn->cn_Size>>2))
				{
					delete points;
					data->errcode = ERR_READFILE;
					return FALSE;
				}
				if(!data->link->SORPoints(data->rc, obj, points, cn->cn_Size>>3))
				{
					delete points;
					data->errcode = ERR_MEM;
					return FALSE;
				}
				delete points;
				break;
		}
	}

	if(data->link->type == LINK_SCENARIO)
	{
		data->link->ObjectAxis(data->rc, obj, &pos, &ox, &oy, &oz, &size);
	}
	else
	{
		data->link->ObjectActor(data->rc, obj, actor);
		data->link->SORAxis(data->rc, obj, &pos, &ox, &oy, &oz, &size);
	}

	return TRUE;
}

/*************
 * DESCRIPTION:   read a RSOB-file
 * INPUT:         rc          context
 *                filename    name of RSOB-file
 *                link        link structure
 *                pos         object position
 *                ox, oy, oz  object orientation
 *                scale       object scale
 *                actor       pointer to actor
 *                replacesurf pointer to surface to replace object surface
 *                            with
 *                version     version number
 * OUTPUT:        NULL if ok else errorstring
 *************/
extern "C" char* SAVEDS objRead(rsiCONTEXT *rc, char* filename, OBJLINK *link, VECTOR *pos, VECTOR *ox, VECTOR *oy, VECTOR *oz,
	VECTOR *scale, ACTOR *actor, SURFACE *replacesurf, ULONG *version, float (*SetProgress)(rsiCONTEXT*, float))
{
	long error = 0;
	struct ContextNode *cn;
	MATRIX_STACK stack;
	BOOL added = FALSE;
	HANDLER_DATA data;
	MATRIX m;

	// initialize handler data
	data.link = link;
	data.rsob_version = NULL;
	data.rc = rc;
	data.pos = *pos;
	data.ox = *ox;
	data.oy = *oy;
	data.oz = *oz;
	data.scale = *scale;
	data.size_done = 0;

	if(data.link->type == LINK_SCENARIO)
	{
		m.SetOTMatrix(ox,oy,oz, pos);
		stack.Push(&m);
	}

	data.iff = AllocIFF();
	if(!data.iff)
		return errors[ERR_MEM];

#ifdef __AMIGA__
	data.iff->iff_Stream = Open(filename, MODE_OLDFILE);
#else
	data.iff->iff_Stream = Open_(filename, MODE_OLDFILE);
#endif
	if (!(data.iff->iff_Stream))
	{
		ReadRSOB_Cleanup(&data);
		return errors[ERR_OPENFILE];
	}

	InitIFFasDOS(data.iff);
	error = OpenIFF(data.iff, IFFF_READ);
	if (error)
	{
		ReadRSOB_Cleanup(&data);
		return errors[ERR_IFFPARSE];
	}

	error = ParseIFF(data.iff, IFFPARSE_RAWSTEP);
	if (error)
	{
		ReadRSOB_Cleanup(&data);
		return errors[ERR_IFFPARSE];
	}
	cn = CurrentChunk(data.iff);
	if(!cn)
	{
		ReadRSOB_Cleanup(&data);
		return errors[ERR_IFFPARSE];
	}
	if((cn->cn_ID != ID_FORM) || (cn->cn_Type != ID_RSOB))
	{
		ReadRSOB_Cleanup(&data);
		return errors[ERR_NORSOBFILE];
	}
	data.filesize = cn->cn_Size;

	while(!error || error == IFFERR_EOC)
	{
		error = ParseIFF(data.iff, IFFPARSE_RAWSTEP);
		if(error != IFFERR_EOC)
		{
			// Get a pointer to the context node describing the current context
			cn = CurrentChunk(data.iff);
			if (cn)
			{
				switch (cn->cn_ID)
				{
					case ID_VERS:
						if(!ReadULONG(data.iff,version,1))
						{
							ReadRSOB_Cleanup(&data);
							return errors[ERR_RSOBFILE];
						}
						if(*version > RSOB_VERSION)
						{
							ReadRSOB_Cleanup(&data);
							return errors[ERR_WRONGVERS];
						}
						data.rsob_version = *version;
						break;
					case ID_FORM:
						switch(cn->cn_Type)
						{
							case ID_SRFS:
								if(!ParseSurfaces(&data, &stack, actor))
								{
									ReadRSOB_Cleanup(&data);
									return errors[data.errcode];
								}
								break;
							case ID_CAMR:
								if(added)
								{
									stack.Pop();
									data.link->ObjectEnd(data.rc);
								}
								if(data.link->type == LINK_SCENARIO)
								{
									if(!ParseCamera(&data,&stack,actor))
									{
										ReadRSOB_Cleanup(&data);
										return errors[data.errcode];
									}
								}
								added = TRUE;
								break;
							case ID_SPHR:
								if(added)
								{
									stack.Pop();
									data.link->ObjectEnd(data.rc);
								}
								if(!ParseSphere(&data,&stack,actor))
								{
									ReadRSOB_Cleanup(&data);
									return errors[data.errcode];
								}
								added = TRUE;
								break;
							case ID_BOX:
								if(added)
								{
									stack.Pop();
									data.link->ObjectEnd(data.rc);
								}
								if(!ParseBox(&data,&stack,actor))
								{
									ReadRSOB_Cleanup(&data);
									return errors[data.errcode];
								}
								added = TRUE;
								break;
							case ID_CYLR:
								if(added)
								{
									stack.Pop();
									data.link->ObjectEnd(data.rc);
								}
								if(!ParseCylinder(&data,&stack,actor))
								{
									ReadRSOB_Cleanup(&data);
									return errors[data.errcode];
								}
								added = TRUE;
								break;
							case ID_CONE:
								if(added)
								{
									stack.Pop();
									data.link->ObjectEnd(data.rc);
								}
								if(!ParseCone(&data,&stack,actor))
								{
									ReadRSOB_Cleanup(&data);
									return errors[data.errcode];
								}
								added = TRUE;
								break;
							case ID_PLAN:
								if(added)
								{
									stack.Pop();
									data.link->ObjectEnd(data.rc);
								}
								if(!ParsePlane(&data,&stack,actor))
								{
									ReadRSOB_Cleanup(&data);
									return errors[data.errcode];
								}
								added = TRUE;
								break;
							case ID_CSG:
								if(added)
								{
									stack.Pop();
									data.link->ObjectEnd(data.rc);
								}
								if(!ParseCSG(&data,&stack,actor))
								{
									ReadRSOB_Cleanup(&data);
									return errors[data.errcode];
								}
								added = TRUE;
								break;
							case ID_PLGT:
							case ID_SLGT:
							case ID_DLGT:
								if(added)
								{
									stack.Pop();
									data.link->ObjectEnd(data.rc);
								}
								if(!ParseLight(&data, cn->cn_Type, &stack, actor))
								{
									ReadRSOB_Cleanup(&data);
									return errors[data.errcode];
								}
								added = TRUE;
								break;
							case ID_MESH:
								if(added)
								{
									stack.Pop();
									data.link->ObjectEnd(data.rc);
								}
								if(!ParseMesh(&data,&stack,actor,pos,ox,oy,oz,scale))
								{
									ReadRSOB_Cleanup(&data);
									return errors[data.errcode];
								}
								added = TRUE;
								break;
							case ID_SOR:
								if(added)
								{
									stack.Pop();
									data.link->ObjectEnd(data.rc);
								}
								if(!ParseSOR(&data,&stack,actor))
								{
									ReadRSOB_Cleanup(&data);
									return errors[data.errcode];
								}
								added = TRUE;
								break;
							case ID_HIER:
								added = FALSE;
								break;
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
		else
		{
			cn = CurrentChunk(data.iff);
			if (cn)
			{
				if((cn->cn_ID == ID_FORM) && (cn->cn_Type == ID_HIER))
				{
					data.link->ObjectEnd(data.rc);
					stack.Pop();
				}
			}
		}
	}
	if (error != IFFERR_EOF)
	{
		return errors[ERR_IFFPARSE];
	}
	if(added)
	{
		stack.Pop();
		data.link->ObjectEnd(data.rc);
	}

	ReadRSOB_Cleanup(&data);

	return NULL;
}

/*************
 * DESCRIPTION:   free mem and iff-handle
 * INPUT:         data        handler data
 * OUTPUT:        none
 *************/
static void ReadRSOB_Cleanup(HANDLER_DATA *data)
{
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
