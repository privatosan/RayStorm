/***************
 * MODUL:         bump
 * NAME:          bump.cpp
 * DESCRIPTION:   Bump Texture
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
	DIALOG_COLGROUP, 2,
		DIALOG_LABEL, DE("X bump depth"),
		DIALOG_FLOAT,
		DIALOG_LABEL, DE("Y bump depth"),
		DIALOG_FLOAT,
		DIALOG_LABEL, DE("Z bump depth"),
		DIALOG_FLOAT,
	DIALOG_END,
	DIALOG_FINISH
};

typedef struct
{
	float xsize, ysize, zsize;
} DIALOG_DATA;

typedef struct
{
	TEXTURE_INFO tinfo;
	short *hashTable;
} BUMP_INFO;

DIALOG_DATA default_data =
{
	1.f, 1.f, 1.f
};

/*************
 * DESCRIPTION:   This is the cleanup function for the textures.
 * INPUT:         tinfo
 * OUTPUT:        pointer to texture info, NULL if failed
 *************/
static void SAVEDS texture_cleanup(BUMP_INFO *tinfo)
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
static void SAVEDS texture_work(BUMP_INFO *info, DIALOG_DATA *params, TEXTURE_PATCH *patch, VECTOR *v)
{
	VECTOR disp, *norm;
	float d;

	DNoise(v, &disp, info->hashTable);

	norm = &patch->norm;
	norm->x += disp.x * params->xsize;
	norm->y += disp.y * params->ysize;
	norm->z += disp.z * params->zsize;

	// Normalize
	d = sqrt(norm->x*norm->x + norm->y*norm->y + norm->z*norm->z);
	if (d == 0.f)
		return;
	d = 1.f/d;
	norm->x *= d;
	norm->y *= d;
	norm->z *= d;
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
	BUMP_INFO *info;

	info = new BUMP_INFO;
	if(info)
	{
		if(!InitNoise(&info->hashTable))
		{
			delete info;
			return NULL;
		}

		info->tinfo.name = "Bump";
		info->tinfo.dialog = dialog;
		info->tinfo.defaultdata = &default_data;
		info->tinfo.datasize = sizeof(DIALOG_DATA);
		info->tinfo.cleanup = (void (*)(TEXTURE_INFO*))texture_cleanup;
		info->tinfo.work = (void (*)(TEXTURE_INFO*, void*, TEXTURE_PATCH*, VECTOR*))texture_work;
	}

	return (TEXTURE_INFO*)info;
}

