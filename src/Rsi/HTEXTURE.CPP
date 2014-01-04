/***************
 * MODUL:         htexture
 * NAME:          htexture.cpp
 * DESCRIPTION:   This file includes all functions for the hypertexture class
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    12.03.97 ah    initial release
 *    20.07.99 ah    changed to loadable texture modules
 ***************/

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

#ifdef __AMIGA__
#ifndef TEXTURE_MODULE_H
#ifdef __STORM__
#include "rtxt:texture_module.h"
#else
#include "texture_module.h"
#endif
#endif
#else // __AMIGA__
typedef TEXTURE_INFO *(*INITPROC)();
#endif // __AMIGA__

/*************
 * DESCRIPTION:   Constructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
HYPER_TEXTURE::HYPER_TEXTURE()
{
	data = NULL;
#ifdef __AMIGA__
	Base = NULL;
#else // __AMIGA__
	hLib = NULL;
#endif // __AMIGA__
}

/*************
 * DESCRIPTION:   Denstructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
HYPER_TEXTURE::~HYPER_TEXTURE()
{
	if(data)
		delete data;
#ifdef __AMIGA__
	if(Base)
	{
		tinfo->cleanup(tinfo);
		CloseLibrary(Base);
	}
#else // __AMIGA__
	if(hLib)
	{
		tinfo->cleanup(tinfo);
		FreeLibrary(hLib);
	}
#endif // __AMIGA__
}

/*************
 * DESCRIPTION:   Load hyper texture and init it
 * INPUT:         filename    texture filename
 *                path        texture path
 * OUTPUT:        ERR_NONE if ok, else error number
 *************/
int HYPER_TEXTURE::Load(RSICONTEXT *rc, char *filename, char *path)
{
	char buffer[256];
#ifndef __AMIGA__
	INITPROC texture_init;
#endif // __AMIGA__

	if(!ExpandPath(path,filename,buffer))
		return ERR_OPENTEXTUREFILE;

#ifdef __AMIGA__
	Base = OpenLibrary(buffer, 0);
	if(!Base)
		return ERR_OPENTEXTUREFILE;
	TextureBase = Base;
#else // __AMIGA__
	if (!(hLib = LoadLibrary(buffer)))
		return ERR_OPENTEXTUREFILE;
	if (!(texture_init = (INITPROC)GetProcAddress(hLib, "texture_init")))
		return ERR_OPENTEXTUREFILE;
#endif // __AMIGA__

	tinfo = texture_init();

	data = new UBYTE[tinfo->datasize];
	if(!data)
	{
		tinfo->cleanup(tinfo);
#ifdef __AMIGA__
		CloseLibrary(Base);
		Base = NULL;
#else // __AMIGA__
		FreeLibrary(hLib);
		hLib = NULL;
#endif // __AMIGA__
		return ERR_MEM;
	}

	// copy default parameters
	memcpy(data, tinfo->defaultdata, tinfo->datasize);

	return ERR_NONE;
}

/*************
 * DESCRIPTION:   Apply hyper texture to surface
 * INPUT:         norm        normal
 *                surf        surface
 *                pos_        current position on surface
 *                ray         current ray
 * OUTPUT:        none
 *************/
void HYPER_TEXTURE::Apply(VECTOR *norm, SURFACE *surf, const VECTOR *pos_, RAY *ray, float *density, float object_density, COLOR *color)
{
	TEXTURE_PATCH ptch;
	VECTOR relpos,v;

	ptch.diffuse = *color;
	ptch.object_density = object_density;
	ptch.norm = *norm;

	// update texture if neccassary
	if(actor && (time != ray->time))
		Update(ray->time);

	VecSub(pos_, &pos, &v);
	relpos.x = dotp(&orient_x, &v) / size.x;
	relpos.y = dotp(&orient_y, &v) / size.y;
	relpos.z = dotp(&orient_z, &v) / size.z;

	tinfo->work(tinfo, data, &ptch, &relpos);

	*color = ptch.diffuse;
	*density = ptch.density;
	*norm = ptch.norm;
}

/*************
 * DESCRIPTION:   duplicate a texture
 * INPUT:         none
 * OUTPUT:        new texture
 *************/
TEXTURE *HYPER_TEXTURE::Duplicate()
{
	HYPER_TEXTURE *texture;

	texture = new HYPER_TEXTURE();
	if(texture)
	{
#ifdef __STORM__
		// Bug in StormC introduced with 2.0.23
		memcpy(texture, this, sizeof(HYPER_TEXTURE));
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

