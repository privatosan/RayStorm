/***************
 * MODUL:         Wood
 * NAME:          wood.cpp
 * DESCRIPTION:   Wood texture
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    02.06.98 ah    initial release
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

#ifndef TEXTURE_MOD_H
#include "rtexture.h"
#endif

#ifndef NOISE_H
#include "noise.h"
#endif

enum
{
	ID_DIFFUSE, ID_REFLECT, ID_TRANSPAR
};

DIALOG_ELEMENT dialog[] =
{
	DIALOG_VGROUP,
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
		DIALOG_COLGROUP, 2,
			DIALOG_LABEL, DE("Octaves"),
			DIALOG_SLIDER, 1, 8,
			DIALOG_LABEL, DE("Frequency"),
			DIALOG_FLOAT,
		DIALOG_END,
	DIALOG_END,
	DIALOG_FINISH
};

typedef struct
{
	COLOR diffuse;
	ULONG diffuse_enable;
	COLOR reflect;
	ULONG reflect_enable;
	COLOR transpar;
	ULONG transpar_enable;
	ULONG octaves;
	float frequency
} DIALOG_DATA;

typedef struct
{
	TEXTURE_INFO tinfo;
	short *hashTable;
} WOOD_INFO;

DIALOG_DATA default_data =
{
	{ .8f, .6f, .2f },
	TRUE,
	{ 0.f, 0.f, 0.f },
	FALSE,
	{ 0.f, 0.f, 0.f },
	FALSE,
	5,
	7
};

/*************
 * DESCRIPTION:   This is the cleanup function for the textures.
 * INPUT:         tinfo
 * OUTPUT:        pointer to texture info, NULL if failed
 *************/
extern "C" void SAVEDS texture_cleanup(WOOD_INFO *tinfo)
{
	CleanupNoise(tinfo->hashTable);
}

/*************
 * DESCRIPTION:   this is the work function
 * INPUT:         info     info structure
 *                params   pointer to user texture parameters
 *                patch    pointer to patch structure
 *                v        hit position - relative to texture axis
 * OUTPUT:        -
 *************/
extern "C" void SAVEDS texture_work(WOOD_INFO *info, DIALOG_DATA *params, TEXTURE_PATCH *patch, VECTOR *v)
{
	float val, one_minus_val;

	val = (float)sin(20.f * Turbulence(v, params->octaves, info->hashTable) + params->frequency * v->x) + 1.f;
	val = pow(0.5f * val, 0.77f);
	one_minus_val = 1.f;

	if(params->diffuse_enable)
	{
		patch->diffuse.r = patch->diffuse.r * one_minus_val + params->diffuse.r * val;
		patch->diffuse.g = patch->diffuse.g * one_minus_val + params->diffuse.g * val;
		patch->diffuse.b = patch->diffuse.b * one_minus_val + params->diffuse.b * val;
	}
	if(params->reflect_enable)
	{
		patch->reflect.r = patch->reflect.r * one_minus_val + params->reflect.r * val;
		patch->reflect.g = patch->reflect.g * one_minus_val + params->reflect.g * val;
		patch->reflect.b = patch->reflect.b * one_minus_val + params->reflect.b * val;
	}
	if(params->transpar_enable)
	{
		patch->transpar.r = patch->transpar.r * one_minus_val + params->transpar.r * val;
		patch->transpar.g = patch->transpar.g * one_minus_val + params->transpar.g * val;
		patch->transpar.b = patch->transpar.b * one_minus_val + params->transpar.b * val;
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
	WOOD_INFO *info;

	info = new WOOD_INFO;
	if(info)
	{
		if(!InitNoise(&info->hashTable))
		{
			delete info;
			return NULL;
		}

		info->tinfo.name = "Wood";
		info->tinfo.dialog = dialog;
		info->tinfo.defaultdata = &default_data;
		info->tinfo.datasize = sizeof(DIALOG_DATA);
		info->tinfo.cleanup = (void (*)(TEXTURE_INFO*))texture_cleanup;
		info->tinfo.work = (void (*)(TEXTURE_INFO*, void*, TEXTURE_PATCH*, VECTOR*))texture_work;
	}

	return (TEXTURE_INFO*)info;
}

