/***************
 * MODUL:         texture
 * NAME:          texture.cpp
 * DESCRIPTION:   This file includes all functions for the texture class
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    23.02.95 ah    first release
 *    15.03.95 ah    texture is now a list
 *    08.04.95 ah    added orientation and length
 *    11.04.95 ah    added linear texture
 *    19.04.95 ah    added support of Imagine textures (only AMIGA)
 *    06.05.95 ah    same textures are not loaded twice now
 *    17.05.95 ah    linear and checker are now own Imagine modules
 *    23.09.95 ah    changed error management
 *    21.06.97 ah    changed to new V2 texture handling
 ***************/

/* Set if you want to debug a texture */
/*#define TEXTTEST*/

#include <string.h>

#ifdef __AMIGA__
	#include <exec/memory.h>
	#include <dos/dos.h>
	#include <pragma/exec_lib.h>
	#include <pragma/dos_lib.h>
	#include "lib.h"
	struct Library *TextureBase;
#else
	#include <windows.h>
#endif /* __AMIGA__ */

#ifndef TEXTURE_H
#include "texture.h"
#endif

#ifndef VECMATH_H
#include "vecmath.h"
#endif

#ifndef SURFACE_H
#include "surface.h"
#endif

#ifndef OBJECT_H
#include "object.h"
#endif

#ifndef LIST_H
#include "list.h"
#endif

#ifndef ERROR_H
#include "error.h"
#endif

#ifndef FILEUTIL_H
#include "fileutil.h"
#endif

#ifndef CONTEXT_H
#include "context.h"
#endif

#ifndef TEXTURE_MODULE_H
#ifdef __STORM__
#include "rtxt:texture_module.h"
#else
#include "texture_module.h"
#endif
#endif

#ifndef __AMIGA__
#ifdef __WATCOMC__
	typedef IM_TTABLE __cdecl *(*INQUIRETEXTURE)(int, int);
#else
	typedef IM_TTABLE *(*INQUIRETEXTURE)(int, int);
#endif
#endif

/*************
 * DESCRIPTION:   Constructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
TEXTURE::TEXTURE()
{
	actor = NULL;
	time = 0.f;
}

/*************
 * DESCRIPTION:   Constructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
IMAGINE_TEXTURE::IMAGINE_TEXTURE()
{
	texture = NULL;
}

/*************
 * DESCRIPTION:   Update texture parameters
 * INPUT:         time     actual time
 * OUTPUT:        none
 *************/
void TEXTURE::Update(const float time)
{
	TIME t;
	MATRIX r;

	if((actor->time.begin != this->time) || (actor->time.end != time))
	{
		t.begin = this->time;
		t.end = time;
		actor->Animate(&t);
	}
	actor->matrix->MultVectMat(&pos);

	actor->rotmatrix->InvMat(&r);
	r.MultVectMat(&orient_x);
	r.MultVectMat(&orient_y);
	r.MultVectMat(&orient_z);

	this->time = time;
}

/*************
 * DESCRIPTION:   Load Imagine texture and init it
 * INPUT:         filename    texture filename
 *                path        texture path
 * OUTPUT:        ERR_NONE if ok, else error number
 *************/
int IMAGINE_TEXTURE::Load(RSICONTEXT *rc, char *filename, char *path)
{
	IM_TTABLE* (*texture_init)(LONG arg0);
	char buffer[256];

	texture = rc->text_root->FindIMTexture(filename);
	if(!texture)
	{
		/* don't found the texture => load it again */
		texture = new IM_TEXT;
		if(!texture)
			return ERR_MEM;

		/* Load Imagine texture  */
		if(!ExpandPath(path,filename,buffer))
		{
			delete texture;
			return ERR_OPENTEXTUREFILE;
		}
#ifdef __AMIGA__
		texture->seglist = LoadSeg(buffer);
		if(!texture->seglist)
		{
			delete texture;
			return ERR_OPENTEXTUREFILE;
		}

		*(ULONG*)(&texture_init) = 4*texture->seglist+4;
#ifdef __PPC__
		texture->ttable = ppc_texture_init(texture_init, 0x60FFFF);
#else
		texture->ttable = texture_init(0x60FFFF);
#endif // __PPC__
#else
		texture->ttable = NULL;

		INQUIRETEXTURE InquireTexture;

		texture->hInst = LoadLibrary(buffer);
		if (!texture->hInst)
		{
			delete texture;
			return ERR_INITTEXTURE;
		}

		InquireTexture = (INQUIRETEXTURE)GetProcAddress(texture->hInst, "InquireTexture");
		if (!InquireTexture)
		{
			delete texture;
			return ERR_INITTEXTURE;
		}

		texture->ttable = InquireTexture(0x70, 0x1);
#endif
		if(!texture->ttable)
		{
#ifndef __WATCOMC__
			delete texture;
#endif
			return ERR_INITTEXTURE;
		}

		texture->name = new char[strlen(filename)+1];
		if(!texture->name)
		{
			delete texture;
			return ERR_MEM;
		}
		strcpy(texture->name,filename);

		texture->Insert(rc);
	}

	/* Copy default parameters */
	memcpy(param, texture->ttable->params, 16*sizeof(float));
	/* Copy default geometry */
	memcpy(&pos, texture->ttable->tform, 5*sizeof(VECTOR));

	return ERR_NONE;
}

/*************
 * DESCRIPTION:   Apply imagine texture to surface
 * INPUT:         norm        normal
 *                surf        surface
 *                pos_        current position on surface
 *                ray         actual ray
 * OUTPUT:        none
 *************/
void IMAGINE_TEXTURE::Apply(VECTOR *norm, SURFACE *surf, const VECTOR *pos_, RAY *ray)
{
	IM_PATCH patch;            /* patch-structure */
	VECTOR v, relpos;          /* hit position relative to texture axis */

	patch.ptc_pos = *pos_;
	patch.ptc_nor = *norm;
	patch.ptc_col = surf->diffuse;
	patch.ptc_ref = surf->reflect;
	patch.ptc_tra = surf->transpar;
	patch.ptc_spc = surf->specular;
	patch.ptc_shp = 2;
	patch.ptc_shd = TRUE;
	patch.ptc_ray = &ray->start;
	patch.raydist = ray->lambda;
	patch.foglen = 0.f;

	// update texture if neccassary
	if(actor && (time != ray->time))
		Update(ray->time);

	VecSub(pos_, &pos, &v);
	relpos.x = dotp(&orient_x, &v);
	relpos.y = dotp(&orient_y, &v);
	relpos.z = dotp(&orient_z, &v);

#ifdef __PPC__
	ppc_texture_work(texture->ttable->work, param, &patch, &relpos, (float*)(&pos));
#else
	texture->ttable->work(param, &patch, &relpos, (float*)(&pos));
#endif

	surf->diffuse = patch.ptc_col;
	surf->reflect = patch.ptc_ref;
	surf->transpar = patch.ptc_tra;
	*norm = patch.ptc_nor;
}

/*************
 * DESCRIPTION:   duplicate a texture
 * INPUT:         none
 * OUTPUT:        new texture
 *************/
TEXTURE *IMAGINE_TEXTURE::Duplicate()
{
	IMAGINE_TEXTURE *texture;

	texture = new IMAGINE_TEXTURE();
	if(texture)
	{
#ifdef __STORM__
		// Bug in StormC introduced with 2.0.23
		memcpy(texture, this, sizeof(IMAGINE_TEXTURE));
#else
		*texture = *this;
#endif // __STORMC__
	}
	return (TEXTURE*)texture;
}

/*************
 * DESCRIPTION:   Constructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
IM_TEXT::IM_TEXT()
{
#ifdef __AMIGA__
	seglist = NULL;
#else
	hInst = 0;
#endif
	ttable = NULL;
	name = NULL;
}

/*************
 * DESCRIPTION:   Destructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
IM_TEXT::~IM_TEXT()
{
	if(ttable)
	{
		if(ttable->cleanup)
		{
#ifdef __PPC__
			ppc_texture_cleanup(ttable->cleanup);
#else
			ttable->cleanup();
#endif // __PPC__
		}
	}
#ifdef __AMIGA__
	if(seglist)
		UnLoadSeg(seglist);
#else
	FreeLibrary(hInst);
#endif /* __AMIGA__ */
	if(name)
		delete name;
}

/*************
 * DESCRIPTION:   Inserts a texture in the texture list
 * INPUT:         none
 * OUTPUT:        none
 *************/
void IM_TEXT::Insert(RSICONTEXT *rc)
{
	rc->Insert(this);
}

/*************
 * FUNCTION:      IM_TEXT::FindIMTexture
 * DESCRIPTION:   find texture with filename 'texturename'
 * INPUT:         texturename    name of texture
 * OUTPUT:        pointer to texture, NULL if non found
 *************/
IM_TEXT* IM_TEXT::FindIMTexture(char *texturename)
{
	IM_TEXT *cur;

	cur = this;
	while(cur)
	{
		if(!strcmp(texturename, cur->name))
			return cur;
		cur = (IM_TEXT*)cur->GetNext();
	}
	return NULL;
}

/*************
 * DESCRIPTION:   Constructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
RAYSTORM_TEXTURE::RAYSTORM_TEXTURE()
{
	data = NULL;
#ifdef __AMIGA__
	Base = NULL;
#endif // __AMIGA__
}

/*************
 * DESCRIPTION:   Denstructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
RAYSTORM_TEXTURE::~RAYSTORM_TEXTURE()
{
	if(data)
		delete data;
#ifdef __AMIGA__
	if(Base)
	{
		tinfo->cleanup(tinfo);
		CloseLibrary(Base);
	}
#endif /* __AMIGA__ */
}

/*************
 * DESCRIPTION:   Load raystorm texture and init it
 * INPUT:         filename    texture filename
 *                path        texture path
 * OUTPUT:        ERR_NONE if ok, else error number
 *************/
int RAYSTORM_TEXTURE::Load(RSICONTEXT *rc, char *filename, char *path)
{
	char buffer[256];

	if(!ExpandPath(path,filename,buffer))
		return ERR_OPENTEXTUREFILE;

#ifdef __AMIGA__
	Base = OpenLibrary(buffer, 0);
	if(!Base)
		return ERR_OPENTEXTUREFILE;
	TextureBase = Base;
#endif // __AMIGA__

	tinfo = texture_init();

	data = new UBYTE[tinfo->datasize];
	if(!data)
	{
		tinfo->cleanup(tinfo);
#ifdef __AMIGA__
		CloseLibrary(Base);
		Base = NULL;
#endif // __AMIGA__
		return ERR_MEM;
	}

	// copy default parameters
	memcpy(data, tinfo->defaultdata, tinfo->datasize);

	return ERR_NONE;
}

/*************
 * DESCRIPTION:   Apply RayStorm texture to surface
 * INPUT:         norm        normal
 *                surf        surface
 *                pos_        current position on surface
 *                ray         actual ray
 * OUTPUT:        none
 *************/
void RAYSTORM_TEXTURE::Apply(VECTOR *norm, SURFACE *surf, const VECTOR *pos_, RAY *ray)
{
	TEXTURE_PATCH ptch;
	VECTOR relpos,v;

	VecSub(pos_, &pos, &v);
	relpos.x = dotp(&orient_x, &v) / size.x;
	relpos.y = dotp(&orient_y, &v) / size.y;
	relpos.z = dotp(&orient_z, &v) / size.z;

	ptch.ambient = surf->ambient;
	ptch.specular = surf->specular;
	ptch.diffuse = surf->diffuse;
	ptch.reflect = surf->reflect;
	ptch.transpar = surf->transpar;
	ptch.difftrans = surf->difftrans;
	ptch.spectrans = surf->spectrans;
	ptch.refphong = surf->refphong;
	ptch.transphong = surf->transphong;
	ptch.foglength = surf->foglength;
	ptch.refrindex = surf->refrindex;
	ptch.translucency = surf->translucency;
	ptch.norm = *norm;

	// update texture if neccassary
	if(actor && (time != ray->time))
		Update(ray->time);

	tinfo->work(tinfo, data, &ptch, &relpos);

	surf->ambient = ptch.ambient;
	surf->specular = ptch.specular;
	surf->diffuse = ptch.diffuse;
	surf->reflect = ptch.reflect;
	surf->transpar = ptch.transpar;
	surf->difftrans = ptch.difftrans;
	surf->spectrans = ptch.spectrans;
	surf->refphong = ptch.refphong;
	surf->transphong = ptch.transphong;
	surf->foglength = ptch.foglength;
	surf->refrindex = ptch.refrindex;
	surf->translucency = ptch.translucency;
	*norm = ptch.norm;
}

/*************
 * DESCRIPTION:   duplicate a texture
 * INPUT:         none
 * OUTPUT:        new texture
 *************/
TEXTURE *RAYSTORM_TEXTURE::Duplicate()
{
	RAYSTORM_TEXTURE *texture;

	texture = new RAYSTORM_TEXTURE();
	if(texture)
	{
#ifdef __STORM__
		// Bug in StormC introduced with 2.0.23
		memcpy(texture, this, sizeof(RAYSTORM_TEXTURE));
#else
		*texture = *this;
#endif // __STORMC__
		if(data)
		{
			texture->data = new UBYTE[tinfo->datasize];
			if(!texture->data)
			{
				delete texture;
				return NULL;
			}
			memcpy(texture->data, data, tinfo->datasize);
		}
	}
	return (TEXTURE*)texture;
}

