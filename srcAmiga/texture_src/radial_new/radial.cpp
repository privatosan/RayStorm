/***************
 * MODUL:         Radial
 * NAME:          radial.cpp
 * DESCRIPTION:   Radial Texture
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    05.09.98 ah    initial release
 ***************/

#ifdef __AMIGA__
#include "texturelib.h"
#endif // __AMIGA__

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef DIALOG_H
#include "dialog.h"
#endif

#ifndef TEXTURE_H
#include "rtexture.h"
#endif

DIALOG_ELEMENT dialog[] =
{
	DIALOG_COLGROUP, 2,
		DIALOG_LABEL, DE("Start radius"),
		DIALOG_FLOAT,
		DIALOG_LABEL, DE("End radius"),
		DIALOG_FLOAT,
	DIALOG_END,
	DIALOG_COLGROUP, 3,
		DIALOG_LABEL, DE("Diffuse"),
		DIALOG_COLORBOX,
		DIALOG_CHECKBOX,
		DIALOG_LABEL, DE("Reflective"),
		DIALOG_COLORBOX,
		DIALOG_CHECKBOX,
		DIALOG_LABEL, DE("Transparent"),
		DIALOG_COLORBOX,
		DIALOG_CHECKBOX,
	DIALOG_END,
	DIALOG_FINISH
};

typedef struct
{
	float start_radius;
	float end_radius;
	COLOR diffuse;
	ULONG diffuse_enabled;
	COLOR reflect;
	ULONG reflect_enabled;
	COLOR transpar;
	ULONG transpar_enabled;
} DIALOG_DATA;

typedef struct
{
	TEXTURE_INFO tinfo;
} RADIAL_INFO;

DIALOG_DATA default_data =
{
	0.f,
	1.f,
	{ 0.f, 0.f, 0.f },
	1,
	{ 0.f, 0.f, 0.f },
	0,
	{ 0.f, 0.f, 0.f },
	0
};

/*************
 * DESCRIPTION:   This is the cleanup function for the textures.
 * INPUT:         tinfo
 * OUTPUT:        pointer to texture info, NULL if failed
 *************/
static void SAVEDS texture_cleanup(TEXTURE_INFO *tinfo)
{
}

/*************
 * DESCRIPTION:   this is the work function
 * INPUT:         info     info structure
 *                params   pointer to user texture parameters
 *                patch    pointer to patch structure
 *                v        hit position - relative to texture axis
 * OUTPUT:        -
 *************/
static void SAVEDS texture_work(RADIAL_INFO *info, DIALOG_DATA *params, TEXTURE_PATCH *patch, VECTOR *v)
{
	float d, one_minus_d;

	d = sqrt(v->x*v->x + v->y*v->y + v->z*v->z);

	if(d <= params->start_radius)
		return;
	if(d >= params->end_radius)
	{
		if(params->diffuse_enabled)
			patch->diffuse = params->diffuse;
		if(params->reflect_enabled)
			patch->reflect = params->reflect;
		if(params->transpar_enabled)
			patch->transpar = params->transpar;
	}
	else
	{
		d = 1.f - (d - params->start_radius)/(params->end_radius - params->start_radius);
		one_minus_d = 1.f - d;
		if(params->diffuse_enabled)
		{
			patch->diffuse.r = params->diffuse.r * d + patch->diffuse.r * one_minus_d;
			patch->diffuse.g = params->diffuse.g * d + patch->diffuse.g * one_minus_d;
			patch->diffuse.b = params->diffuse.b * d + patch->diffuse.b * one_minus_d;
		}
		if(params->reflect_enabled)
		{
			patch->reflect.r = params->reflect.r * d + patch->reflect.r * one_minus_d;
			patch->reflect.g = params->reflect.g * d + patch->reflect.g * one_minus_d;
			patch->reflect.b = params->reflect.b * d + patch->reflect.b * one_minus_d;
		}
		if(params->transpar_enabled)
		{
			patch->transpar.r = params->transpar.r * d + patch->transpar.r * one_minus_d;
			patch->transpar.g = params->transpar.g * d + patch->transpar.g * one_minus_d;
			patch->transpar.b = params->transpar.b * d + patch->transpar.b * one_minus_d;
		}
	}
}

/*************
 * DESCRIPTION:   This is the init function for the textures.
 * INPUT:         -
 * OUTPUT:        pointer to texture info, NULL if failed
 *************/
#ifdef __PPC__
extern "C" TEXTURE_INFO* SAVEDS texture_init(struct Library *TextureBase)
#else
extern "C" TEXTURE_INFO* texture_init()
#endif // __AMIGA__
{
	RADIAL_INFO *info;

	info = new RADIAL_INFO;
	if(info)
	{
		info->tinfo.name = "Radial";
		info->tinfo.dialog = dialog;
		info->tinfo.defaultdata = &default_data;
		info->tinfo.datasize = sizeof(DIALOG_DATA);
		info->tinfo.cleanup = texture_cleanup;
		info->tinfo.work = (void (*)(TEXTURE_INFO*, void*, TEXTURE_PATCH*, VECTOR*))texture_work;
	}

	return (TEXTURE_INFO*)info;
}

