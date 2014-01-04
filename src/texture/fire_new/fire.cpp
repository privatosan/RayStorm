/***************
 * MODUL:         fire
 * NAME:          fire.cpp
 * DESCRIPTION:   Fire Texture
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    16.06.98 ah    initial release
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

#ifndef NOISE_H
#include "noise.h"
#endif

DIALOG_ELEMENT dialog[] =
{
	DIALOG_VGROUP,
		DIALOG_COLGROUP, 2,
			DIALOG_LABEL, DE("Top"),
			DIALOG_COLORBOX,
			DIALOG_LABEL, DE("Middle"),
			DIALOG_COLORBOX,
			DIALOG_LABEL, DE("Bottom"),
			DIALOG_COLORBOX,
		DIALOG_END,
		DIALOG_COLGROUP, 4,
			DIALOG_LABEL, DE("Transparent top"),
			DIALOG_CHECKBOX,
			DIALOG_LABEL, DE("Octaves"),
			DIALOG_SLIDER, 1, 8,
		DIALOG_END,
		DIALOG_COLGROUP, 4,
			DIALOG_LABEL, DE("Chaos scale"),
			DIALOG_FLOAT,
			DIALOG_LABEL, DE("Chaos offset"),
			DIALOG_FLOAT,
			DIALOG_LABEL, DE("Flame height"),
			DIALOG_FLOAT,
			DIALOG_LABEL, DE("Flame amplitude"),
			DIALOG_FLOAT,
		DIALOG_END,
	DIALOG_END,
	DIALOG_FINISH
};

typedef struct
{
	COLOR top, middle, bottom;
	ULONG transparent;
	ULONG octaves;
	float chaos_scale, chaos_offset, flame_height, flame_amplitude;
} DIALOG_DATA;

typedef struct
{
	TEXTURE_INFO tinfo;
	short *hashTable;
} FIRE_INFO;

DIALOG_DATA default_data =
{
	{ 1., 0., 0.},
	{ 1., 1., 0.},
	{ 1., 1., 1.},
	0,
	3,
	1.,
	0.,
	1.,
	.5
};

/*************
 * DESCRIPTION:   This is the cleanup function for the textures.
 * INPUT:         tinfo
 * OUTPUT:        pointer to texture info, NULL if failed
 *************/
static void SAVEDS texture_cleanup(FIRE_INFO *tinfo)
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
static void SAVEDS texture_work(FIRE_INFO *info, DIALOG_DATA *params, TEXTURE_PATCH *patch, VECTOR *v)
{
	VECTOR p;
	float chaos, val;

	p.x = v->x;
	p.y = v->y;
	p.z = exp(v->z);

	chaos = fBm(&p, 1.f, 2.f, params->octaves, info->hashTable);

	chaos = fabs(params->chaos_scale*chaos + params->chaos_offset);

	val = 0.85f*chaos + 0.8f*(params->flame_amplitude - params->flame_height*(p.z - params->flame_height));

	if(val < 0.f)
	{
		// top color below 0
		patch->diffuse.r += params->top.r;
		patch->diffuse.g += params->top.g;
		patch->diffuse.b += params->top.b;

		if(params->transparent)
		{
			// and fully transparent
			patch->transpar.r += 1.f;
			patch->transpar.g += 1.f;
			patch->transpar.b += 1.f;
		}
	}
	else
	{
		if(val > 1.f)
		{
			// bottom color above 1
			patch->diffuse.r += params->bottom.r;
			patch->diffuse.g += params->bottom.g;
			patch->diffuse.b += params->bottom.b;
		}
		else
		{
			if(params->transparent)
			{
				patch->transpar.r += 1.f - val;
				patch->transpar.g += 1.f - val;
				patch->transpar.b += 1.f - val;
			}

			// top
			patch->diffuse.r += params->top.r*(1.f - val);
			patch->diffuse.g += params->top.g*(1.f - val);
			patch->diffuse.b += params->top.b*(1.f - val);

			// middle
			if(val > .5)
			{
				patch->diffuse.r += params->middle.r*(1.f - val)*2.f;
				patch->diffuse.g += params->middle.g*(1.f - val)*2.f;
				patch->diffuse.b += params->middle.b*(1.f - val)*2.f;
			}
			else
			{
				patch->diffuse.r += params->middle.r*val*2.f;
				patch->diffuse.g += params->middle.g*val*2.f;
				patch->diffuse.b += params->middle.b*val*2.f;
			}

			// bottom
			patch->diffuse.r += params->bottom.r*val;
			patch->diffuse.g += params->bottom.g*val;
			patch->diffuse.b += params->bottom.b*val;
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
	FIRE_INFO *info;

	info = new FIRE_INFO;
	if(info)
	{
		if(!InitNoise(&info->hashTable))
		{
			delete info;
			return NULL;
		}

		info->tinfo.name = "Fire";
		info->tinfo.dialog = dialog;
		info->tinfo.defaultdata = &default_data;
		info->tinfo.datasize = sizeof(DIALOG_DATA);
		info->tinfo.cleanup = (void (*)(TEXTURE_INFO*))texture_cleanup;
		info->tinfo.work = (void (*)(TEXTURE_INFO*, void*, TEXTURE_PATCH*, VECTOR*))texture_work;
	}

	return (TEXTURE_INFO*)info;
}

