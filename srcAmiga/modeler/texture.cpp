/***************
 * PROGRAM:       Modeler
 * NAME:          texture.cpp
 * DESCRIPTION:   functions for texture class
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    03.07.96 ah    initial release
 *    18.12.96 ah    added GetSize()
 *    10.06.97 ah    added TEXTURE_OBJECT
 ***************/

#include <string.h>

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef TEXTURE_H
#include "texture.h"
#endif

#ifndef RSCN_H
#include "rscn.h"
#endif

#ifndef UTILITY_H
#include "utility.h"
#endif

#ifndef PREFS_H
#include "prefs.h"
#endif

#ifndef PROJECT_H
#include "project.h"
#endif

#ifdef __AMIGA__
static struct NewMenu popupmenu[] =
{
	{ NM_TITLE, NULL              , 0 ,0,0,(APTR)POP_NONE},
	{ NM_ITEM , "Move"            , 0 ,0,0,(APTR)POP_OBJECT_MOVE},
	{ NM_ITEM , "Rotate"          , 0 ,0,0,(APTR)POP_OBJECT_ROTATE},
	{ NM_ITEM , "Scale"           , 0 ,0,0,(APTR)POP_OBJECT_SCALE},
	{ NM_ITEM , "Settings..."     , 0 ,0,0,(APTR)POP_SETTINGS},
	{ NM_END,NULL,0,0,0,(APTR)0 },
};
#endif

/*************
 * DESCRIPTION:   constructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
TEXTURE::TEXTURE()
{
	name = NULL;
}

/*************
 * DESCRIPTION:   destructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
TEXTURE::~TEXTURE()
{
	if(name)
		delete name;
}

/*************
 * DESCRIPTION:   set texture file name
 * INPUT:         file     new name
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL TEXTURE::SetName(char *file)
{
	if(name)
		delete name;

	name = new char[strlen(file)+1];
	if(!name)
		return FALSE;

	strcpy(name, file);
	return TRUE;
}

/*************
 * DESCRIPTION:   Get size needed of a texture
 * INPUT:         -
 * OUTPUT:        size
 *************/
int TEXTURE::GetSize()
{
	return sizeof(TEXTURE) + strlen(name) + 1;
}


/*************
 * DESCRIPTION:   write texture to scene file
 * INPUT:         iff      iff handler
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL RAYSTORM_TEXTURE::Write(struct IFFHandle *iff)
{
	if(PushChunk(iff, ID_RTXT, ID_FORM, IFFSIZE_UNKNOWN))
		return FALSE;

	if(!WriteChunk(iff, ID_NAME, name, strlen(name)+1))
		return FALSE;

	if(!WriteChunk(iff, ID_PARM, data, datasize))
		return FALSE;

	if(PopChunk(iff))
		return FALSE;

	return TRUE;
}

/*************
 * DESCRIPTION:   constructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
RAYSTORM_TEXTURE::RAYSTORM_TEXTURE()
{
	data = NULL;
}

/*************
 * DESCRIPTION:   destructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
RAYSTORM_TEXTURE::~RAYSTORM_TEXTURE()
{
	if(data)
		delete data;
}

/*************
 * DESCRIPTION:   parse a texture from a RSCN file
 * INPUT:         iff      iff handle
 *                surf     surface to add texture to
 * OUTPUT:        FALSE if failed else TRUE
 *************/
TEXTURE *ParseRayStormTexture(struct IFFHandle *iff, SURFACE *surf)
{
	RAYSTORM_TEXTURE *texture;
	struct ContextNode *cn;
	long error = 0;
	char buffer[256];

	texture = new RAYSTORM_TEXTURE;
	if(!texture)
		return NULL;

	while(!error)
	{
		error = ParseIFF(iff, IFFPARSE_RAWSTEP);
		if(error < 0 && error != IFFERR_EOC)
		{
			delete texture;
			return NULL;
		}

		// Get a pointer to the context node describing the current context
		cn = CurrentChunk(iff);
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
				if(!ReadString(iff,buffer,256))
				{
					delete texture;
					return NULL;
				}
				if(!texture->SetName(buffer))
				{
					delete texture;
					return NULL;
				}
				break;
			case ID_PARM:
				texture->datasize = cn->cn_Size;
				texture->data = new UBYTE[texture->datasize];
				if(!texture->data)
				{
					delete texture;
					return NULL;
				}
				if(!ReadChunkBytes(iff, texture->data, texture->datasize))
				{
					delete texture;
					return NULL;
				}
				break;
		}
	}

	surf->AddTexture(texture);

	return (TEXTURE*)texture;
}

/*************
 * DESCRIPTION:   duplicate object
 * INPUT:         -
 * OUTPUT:        new object or NULL if failed
 *************/
TEXTURE *RAYSTORM_TEXTURE::DupObj()
{
	RAYSTORM_TEXTURE *dup;

	dup = new RAYSTORM_TEXTURE;
	if(!dup)
		return NULL;
	*dup = *this;

	dup->name = NULL;
	dup->data = NULL;

	if(!dup->SetName(name))
	{
		delete dup;
		return NULL;
	}

	dup->data = new UBYTE[datasize];
	if(!dup->data)
	{
		delete dup;
		return NULL;
	}
	memcpy(dup->data, data, datasize);

	return (TEXTURE*)dup;
}

/*************
 * DESCRIPTION:   transfer texture data to RayStorm Interface
 * INPUT:         texture  created texture is stored here
 *                surf     surface
 * OUTPUT:        rsiERR_NONE if ok else error number
 *************/
rsiResult RAYSTORM_TEXTURE::ToRSI(rsiCONTEXT *rc, void **texture, void *surf)
{
	rsiResult err;
	BOOL retry;
	char buffer[256];

	retry = FALSE;
	do
	{
		err = PPC_STUB(rsiAddRayStormTexture)(CTXT, texture, GetName(), surf);
		if(err)
		{
			// can't load texture -> ask user for other filename
			PPC_STUB(rsiGetErrorMsg)(CTXT, buffer, err);
			retry = utility.AskRequest("Can't load texture '%s':\n%s\n\nDo you want to change the filename?", GetName(), buffer);
			if(!retry)
				return err;
			else
			{
				strcpy(buffer, GetName());
				retry = utility.FileRequ(buffer, FILEREQU_RTEXTURE, FILEREQU_INITIALFILEDIR);
				if(retry)
					SetName(buffer);
				else
					return err;
			}
		}
	}
	while(retry && err);

	return PPC_STUB(rsiSetTexture)(CTXT, *texture,
		rsiTTextureParams,   data,
		rsiTDone);
}

/*************
 * DESCRIPTION:   write texture to scene file
 * INPUT:         iff      iff handler
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL HYPER_TEXTURE::Write(struct IFFHandle *iff)
{
	if(PushChunk(iff, ID_HTXT, ID_FORM, IFFSIZE_UNKNOWN))
		return FALSE;

	if(!WriteChunk(iff, ID_NAME, name, strlen(name)+1))
		return FALSE;

	if(PopChunk(iff))
		return FALSE;

	return TRUE;
}

/*************
 * DESCRIPTION:   parse a texture from a RSCN file
 * INPUT:         iff      iff handle
 *                surf     surface to add texture to
 * OUTPUT:        FALSE if failed else TRUE
 *************/
TEXTURE *ParseHyperTexture(struct IFFHandle *iff, SURFACE *surf)
{
	HYPER_TEXTURE *texture;
	struct ContextNode *cn;
	long error = 0;
	char buffer[256];

	texture = new HYPER_TEXTURE;
	if(!texture)
		return NULL;

	while(!error)
	{
		error = ParseIFF(iff, IFFPARSE_RAWSTEP);
		if(error < 0 && error != IFFERR_EOC)
		{
			delete texture;
			return NULL;
		}

		// Get a pointer to the context node describing the current context
		cn = CurrentChunk(iff);
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
				if(!ReadString(iff,buffer,256))
				{
					delete texture;
					return NULL;
				}
				if(!texture->SetName(buffer))
				{
					delete texture;
					return NULL;
				}
				break;
		}
	}

	surf->AddTexture(texture);

	return (TEXTURE*)texture;
}

/*************
 * DESCRIPTION:   duplicate object
 * INPUT:         -
 * OUTPUT:        new object or NULL if failed
 *************/
TEXTURE *HYPER_TEXTURE::DupObj()
{
	HYPER_TEXTURE *dup;

	dup = new HYPER_TEXTURE;
	if(!dup)
		return NULL;
	*dup = *this;
	dup->name = NULL;
	if(!dup->SetName(name))
	{
		delete dup;
		return NULL;
	}
	return (TEXTURE*)dup;
}

/*************
 * DESCRIPTION:   transfer texture data to RayStorm Interface
 * INPUT:         texture  created texture is stored here
 *                surf     surface
 * OUTPUT:        rsiERR_NONE if ok else error number
 *************/
rsiResult HYPER_TEXTURE::ToRSI(rsiCONTEXT *rc, void **texture, void *surf)
{
	return rsiERR_NONE;
}

/*************
 * DESCRIPTION:   write texture to scene file
 * INPUT:         iff      iff handler
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL IMAGINE_TEXTURE::Write(struct IFFHandle *iff)
{
	if(PushChunk(iff, ID_ITXT, ID_FORM, IFFSIZE_UNKNOWN))
		return FALSE;

	if(!WriteChunk(iff, ID_NAME, name, strlen(name)+1))
		return FALSE;

	if(!WriteLongChunk(iff, ID_PARM, &params, 16))
		return FALSE;

	if(PopChunk(iff))
		return FALSE;

	return TRUE;
}

/*************
 * DESCRIPTION:   parse a texture from a RSCN file
 * INPUT:         iff      iff handle
 *                surf     surface to add texture to
 * OUTPUT:        FALSE if failed else TRUE
 *************/
TEXTURE *ParseImagineTexture(struct IFFHandle *iff, SURFACE *surf)
{
	IMAGINE_TEXTURE *texture;
	struct ContextNode *cn;
	long error = 0;
	char buffer[256];

	texture = new IMAGINE_TEXTURE;
	if(!texture)
		return NULL;

	while(!error)
	{
		error = ParseIFF(iff, IFFPARSE_RAWSTEP);
		if(error < 0 && error != IFFERR_EOC)
		{
			delete texture;
			return NULL;
		}

		// Get a pointer to the context node describing the current context
		cn = CurrentChunk(iff);
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
				if(!ReadString(iff,buffer,256))
				{
					delete texture;
					return NULL;
				}
				if(!texture->SetName(buffer))
				{
					delete texture;
					return NULL;
				}
				break;
			case ID_PARM:
				if(!ReadFloat(iff,texture->params,16))
				{
					delete texture;
					return NULL;
				}
				break;
		}
	}

	surf->AddTexture(texture);

	return (TEXTURE*)texture;
}

/*************
 * DESCRIPTION:   duplicate object
 * INPUT:         -
 * OUTPUT:        new object or NULL if failed
 *************/
TEXTURE *IMAGINE_TEXTURE::DupObj()
{
	IMAGINE_TEXTURE *dup;

	dup = new IMAGINE_TEXTURE;
	if(!dup)
		return NULL;
	*dup = *this;
	dup->name = NULL;
	if(!dup->SetName(name))
	{
		delete dup;
		return NULL;
	}
	return (TEXTURE*)dup;
}

/*************
 * DESCRIPTION:   transfer texture data to RayStorm Interface
 * INPUT:         texture  created texture is stored here
 *                surf     surface
 * OUTPUT:        rsiERR_NONE if ok else error number
 *************/
rsiResult IMAGINE_TEXTURE::ToRSI(rsiCONTEXT *rc, void **texture, void *surf)
{
	rsiResult err;
	BOOL retry;
	char buffer[256];

	retry = FALSE;
	do
	{
		err = PPC_STUB(rsiAddImagineTexture)(CTXT, texture, GetName(), surf);
		if(err)
		{
			// can't load texture -> ask user for other filename
			PPC_STUB(rsiGetErrorMsg)(CTXT, buffer, err);
			retry = utility.AskRequest("Can't load texture '%s':\n%s\n\nDo you want to change the filename?", GetName(), buffer);
			if(!retry)
				return err;
			else
			{
				strcpy(buffer, GetName());
				retry = utility.FileRequ(buffer, FILEREQU_ITEXTURE, FILEREQU_INITIALFILEDIR);
				if(retry)
					SetName(buffer);
				else
					return err;
			}
		}
	}
	while(retry && err);

	return PPC_STUB(rsiSetTexture)(CTXT, *texture,
		rsiTTexturePara1,    params[0],
		rsiTTexturePara2,    params[1],
		rsiTTexturePara3,    params[2],
		rsiTTexturePara4,    params[3],
		rsiTTexturePara5,    params[4],
		rsiTTexturePara6,    params[5],
		rsiTTexturePara7,    params[6],
		rsiTTexturePara8,    params[7],
		rsiTTexturePara9,    params[8],
		rsiTTexturePara10,   params[9],
		rsiTTexturePara11,   params[10],
		rsiTTexturePara12,   params[11],
		rsiTTexturePara13,   params[12],
		rsiTTexturePara14,   params[13],
		rsiTTexturePara15,   params[14],
		rsiTTexturePara16,   params[15],
		rsiTDone);
}

/*************
 * DESCRIPTION:   constructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
TEXTURE_OBJECT::TEXTURE_OBJECT()
{
	// Don't place texture on 0,0,0 because of rounding problems with the checker texture
	SetVector(&pos, 0.02, 0.02, 0.02);
	texture = NULL;
}

/*************
 * DESCRIPTION:   gets the popuplist for the selected object
 * INPUT:         -
 * OUTPUT:        popup list
 *************/
#ifdef __AMIGA__
struct NewMenu *TEXTURE_OBJECT::GetPopUpList()
{
	return popupmenu;
}
#endif

/*************
 * DESCRIPTION:   Tests if an object is visible
 * INPUT:         display
 * OUTPUT:        TRUE if visible
 *************/
BOOL TEXTURE_OBJECT::IsVisible(DISPLAY *display)
{
	return(display->filter_flags & FILTER_TEXTURE);
}

/*************
 * DESCRIPTION:   generate bounding box
 * INPUT:         none
 * OUTPUT:        none
 *************/
void TEXTURE_OBJECT::CalcBBox()
{
	bboxmax = size;
	bboxmin.x = -size.x;
	bboxmin.y = -size.y;
	bboxmin.z = -size.z;
}

/*************
 * DESCRIPTION:   Set the color with wich the object is drawn
 * INPUT:         -
 * OUTPUT:        -
 *************/
void TEXTURE_OBJECT::SetDrawColor(DISPLAY *display)
{
#ifdef __OPENGL__
	if(display->display == DISPLAY_SOLID)
		SetGLColor(COLOR_BRUSH);
	else
#endif // __OPENGL__
	{
		if(selected)
			gfx.SetPen(COLOR_BRUSH);
		else
			gfx.SetPen(COLOR_UNSELECTED);
	}
}

/*************
 * DESCRIPTION:   Draw a texture
 * INPUT:         disp     display class
 *                stack    matrix stack
 * OUTPUT:        none
 *************/
void TEXTURE_OBJECT::Draw(DISPLAY *disp,MATRIX_STACK *stack)
{
	switch(disp->display)
	{
		case DISPLAY_BBOX:
		case DISPLAY_WIRE:
			// draw a bounding box around pos
			disp->DrawBox(stack,&bboxmin,&bboxmax);
			break;
	}
}

/*************
 * DESCRIPTION:   sets the new object specs
 * INPUT:         disp     pointer to display structure
 *                pos      translate factor
 *                ox,oy,oz rotate factor
 *                size     scale factor
 * OUTPUT:        none
 *************/
void TEXTURE_OBJECT::SetObject(DISPLAY *disp, VECTOR *pos, VECTOR *ox, VECTOR *oy, VECTOR *oz, VECTOR *size)
{
	CalcBBox();
}

/*************
 * DESCRIPTION:   write texture to scene file
 * INPUT:         iff      iff handler
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL TEXTURE_OBJECT::WriteObject(struct IFFHandle *iff)
{
	VECTOR v[3];

	if(PushChunk(iff, ID_ITXT, ID_FORM, IFFSIZE_UNKNOWN))
		return FALSE;

	if(!WriteLongChunk(iff, ID_POSI, &pos, 3))
		return FALSE;

	v[0] = orient_x;
	v[1] = orient_y;
	v[2] = orient_z;
	if(!WriteLongChunk(iff, ID_ALGN, v, 9))
		return FALSE;

	if(!WriteLongChunk(iff, ID_SIZE, &size, 3))
		return FALSE;

	if(!WriteTrack(iff))
		return FALSE;

	if(!WriteFlags(iff))
		return FALSE;

	if(PopChunk(iff))
		return FALSE;

	return TRUE;
}

/*************
 * DESCRIPTION:   parse a texture from a RSCN file
 * INPUT:         iff      iff handle
 *                obj      object to add texture to
 * OUTPUT:        FALSE if failed else TRUE
 *************/
TEXTURE_OBJECT *ParseTextureObject(struct IFFHandle *iff, OBJECT *obj)
{
	TEXTURE_OBJECT *texture;
	struct ContextNode *cn;
	long error = 0;

	texture = new TEXTURE_OBJECT;
	if(!texture)
		return NULL;

	if(!texture->ReadAxis(iff))
	{
		delete texture;
		return NULL;
	}

	if(rscn_version >= 200)
	{
		while(!error)
		{
			error = ParseIFF(iff, IFFPARSE_RAWSTEP);
			if(error < 0 && error != IFFERR_EOC)
			{
				delete texture;
				return NULL;
			}

			// Get a pointer to the context node describing the current context
			cn = CurrentChunk(iff);
			if(!cn)
				continue;

			if((cn->cn_ID == ID_FORM) && (cn->cn_Type == ID_ITXT))
				break;

			if(error == IFFERR_EOC)
			{
				error = 0;
				continue;
			}
			switch (cn->cn_ID)
			{
				case ID_TRCK:
					if(!texture->ReadTrack(iff))
					{
						delete texture;
						return NULL;
					}
					break;
				case ID_FLGS:
					if(!texture->ReadFlags(iff))
					{
						delete texture;
						return NULL;
					}
					break;
			}
		}
	}

	obj->AddTextureTop(texture);

	return texture;
}

/*************
 * DESCRIPTION:   transfer texture data to RayStorm Interface
 * INPUT:         surf     current surface
 *                obj      current object
 *                stack    matrix stack
 * OUTPUT:        rsiERR_NONE if ok else error number
 *************/
rsiResult TEXTURE_OBJECT::ToRSI(rsiCONTEXT *rc, void *surf, OBJECT *obj, MATRIX_STACK *stack)
{
	rsiResult err;
	void *texture;
	VECTOR ox,oy,oz;
	VECTOR orient_x, orient_y, orient_z, pos;
	MATRIX m;

	err = this->texture->ToRSI(rc, &texture, surf);
	if(err)
		return err;

	m.SetOTMatrix(&this->orient_x,&this->orient_y,&this->orient_z, &this->pos);
	stack->Push(&m);

	stack->GenerateAxis(&orient_x, &orient_y, &orient_z, &pos);

	stack->Pop();

	InvOrient(&orient_x, &orient_y, &orient_z, &ox, &oy, &oz);

	return PPC_STUB(rsiSetTexture)(CTXT, texture,
		rsiTTexturePos,      &pos,
		rsiTTextureSize,     &size,
		rsiTTextureOrientX,  &ox,
		rsiTTextureOrientY,  &oy,
		rsiTTextureOrientZ,  &oz,
		rsiTDone);
}

/*************
 * DESCRIPTION:   duplicate object
 * INPUT:         -
 * OUTPUT:        new object or NULL if failed
 *************/
OBJECT *TEXTURE_OBJECT::DupObj()
{
	TEXTURE_OBJECT *dup;

	dup = new TEXTURE_OBJECT;
	if(!dup)
		return NULL;
	*dup = *this;
	return (OBJECT*)dup;
}

/*************
 * DESCRIPTION:   Get size needed of a texture
 * INPUT:         -
 * OUTPUT:        size
 *************/
int TEXTURE_OBJECT::GetSize()
{
	return sizeof(TEXTURE_OBJECT);
}
