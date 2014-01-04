/***************
 * MODUL:         Clouds
 * NAME:          clouds.cpp
 * DESCRIPTION:   Clouds hyper texture
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    21.07.99 ah    initial release
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
		DIALOG_COLGROUP, 4,
			DIALOG_LABEL, DE("Frequency"),
			DIALOG_FLOAT,
			DIALOG_LABEL, DE("Amplitude"),
			DIALOG_FLOAT,
			DIALOG_LABEL, DE("Octaves"),
			DIALOG_SLIDER, 1, 8,
		DIALOG_END,
	DIALOG_END,
	DIALOG_FINISH
};

typedef struct
{
	float frequency, amplitude;
	ULONG octaves;
} DIALOG_DATA;

typedef struct
{
	TEXTURE_INFO tinfo;
	short *hashTable;
} CLOUDS_INFO;

DIALOG_DATA default_data =
{
	3.f,
	2.f,
	3
};

/*************
 * DESCRIPTION:   This is the cleanup function for the textures.
 * INPUT:         tinfo
 * OUTPUT:        pointer to texture info, NULL if failed
 *************/
static void SAVEDS texture_cleanup(CLOUDS_INFO *tinfo)
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
static void SAVEDS texture_work(CLOUDS_INFO *info, DIALOG_DATA *params, TEXTURE_PATCH *patch, VECTOR *v)
{
	VECTOR p;
	float t;

	VecScale(params->frequency, v, &p);
	t = params->amplitude * Turbulence(&p, params->octaves, info->hashTable);
	if(t < 0.f)
		t = 0.f;

	patch->density = t;
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
	CLOUDS_INFO *info;

	info = new CLOUDS_INFO;
	if(info)
	{
		if(!InitNoise(&info->hashTable))
		{
			delete info;
			return NULL;
		}

		info->tinfo.name = "Clouds";
		info->tinfo.dialog = dialog;
		info->tinfo.defaultdata = &default_data;
		info->tinfo.datasize = sizeof(DIALOG_DATA);
		info->tinfo.flags = FLAG_HYPER_SHADED;
		info->tinfo.cleanup = (void (*)(TEXTURE_INFO*))texture_cleanup;
		info->tinfo.work = (void (*)(TEXTURE_INFO*, void*, TEXTURE_PATCH*, VECTOR*))texture_work;
	}

	return (TEXTURE_INFO*)info;
}

