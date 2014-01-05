/***************
 * MODUL:         surface
 * NAME:          surface.cpp
 * DESCRIPTION:   This file includes the function for the surfaces
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    22.02.95 ah    initial release
 *    15.03.95 ah    texture is now a list
 *    22.03.95 ah    added brushlist
 *    03.04.95 ah    surface is now a list
 *    16.08.95 ah    added flags
 *    12.03.97 ah    added hyper texture
 ***************/

#include <string.h>

#ifndef SURFACE_H
#include "surface.h"
#endif

#ifndef TEXTURE_H
#include "texture.h"
#endif

#ifndef HTEXTURE_H
#include "htexture.h"
#endif

#ifndef CONTEXT_H
#include "context.h"
#endif

/*************
 * DESCRIPTION:   constructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
SURFACE::SURFACE()
{
	texture = NULL;
	hypertexture = NULL;
	brush = NULL;
	flags = 0;
	ambient.r = ambient.g = ambient.b = 1.f;
	diffuse.r = diffuse.g = diffuse.b = 1.f;
	specular.r = specular.g = specular.b = 1.f;
	reflect.r = reflect.g = reflect.b = 0.f;
	transpar.r = transpar.g = transpar.b = 0.f;
	difftrans.r = difftrans.g = difftrans.b = 0.f;
	spectrans.r = spectrans.g = spectrans.b = 1.f;
	refphong = 12.f;
	transphong = 12.f;
	foglength = 0.f;
	refrindex = 1.f;
	translucency = 0.f;
}

/*************
 * DESCRIPTION:   destructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
SURFACE::~SURFACE()
{
	TEXTURE *next,*prev;
	HYPERTEXTURE *hnext,*hprev;
	BRUSH *bnext,*bprev;

	if(texture)
	{
		prev = texture;
		do
		{
			next = (TEXTURE*)prev->GetNext();
			delete prev;
			prev = next;
		}
		while(next);
		texture = NULL;
	}
	if(hypertexture)
	{
		hprev = hypertexture;
		do
		{
			hnext = (HYPERTEXTURE*)hprev->GetNext();
			delete hprev;
			hprev = hnext;
		}
		while(hnext);
		hypertexture = NULL;
	}
	if(brush)
	{
		bprev = brush;
		do
		{
			bnext = (BRUSH*)bprev->GetNext();
			delete bprev;
			bprev = bnext;
		}
		while(bnext);
		texture = NULL;
	}
}

/*************
 * DESCRIPTION:   Inserts a surface in the surface list
 * INPUT:         none
 * OUTPUT:        none
 *************/
void SURFACE::Insert(RSICONTEXT *rc)
{
	rc->Insert(this);
}

/*************
 * DESCRIPTION:   Insert a brush in the brush list of the surface
 * INPUT:         brush
 * OUTPUT:        none
 *************/
void SURFACE::AddBrush(BRUSH *brush)
{
	brush->LIST::Append((LIST**)&this->brush);
}

/*************
 * DESCRIPTION:   Insert a brush in the texture list of the surface
 * INPUT:         texture
 * OUTPUT:        none
 *************/
void SURFACE::AddTexture(TEXTURE *texture)
{
	texture->LIST::Append((LIST**)&this->texture);
}

/*************
 * DESCRIPTION:   Insert a hyper texture
 * INPUT:         texture  pointer to hyper texture
 * OUTPUT:        none
 *************/
void SURFACE::AddHyperTexture(HYPERTEXTURE *texture)
{
	texture->LIST::Insert((LIST**)&this->hypertexture);
}
