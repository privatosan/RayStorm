/***************
 * MODUL:         Stars
 * NAME:          stars.cpp
 * DESCRIPTION:   Stars Texture
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
		DIALOG_LABEL, DE("Density"),
		DIALOG_FLOAT,
	DIALOG_END,
	DIALOG_FINISH
};

typedef struct
{
	COLOR diffuse;
	ULONG diffuse_enabled;
	COLOR reflect;
	ULONG reflect_enabled;
	COLOR transpar;
	ULONG transpar_enabled;
	float density;
} DIALOG_DATA;

typedef struct
{
	TEXTURE_INFO tinfo;
} STARS_INFO;

DIALOG_DATA default_data =
{
	{ 1.f, 1.f, 1.f },
	1,
	{ 0.f, 0.f, 0.f },
	0,
	{ 0.f, 0.f, 0.f },
	0,
	0.01f
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
static void SAVEDS texture_work(STARS_INFO *info, DIALOG_DATA *params, TEXTURE_PATCH *patch, VECTOR *v)
{
	float f;
	f = (v->x*1.23456789f+98765.4321f) * (v->y*2.3456789f+87654.321f) * (v->z*3.456789f+76543.21f);
	f -= floor(f);
	f = f < 0.f ? -f : f;
	if(f < params->density)
	{
		if(params->diffuse_enabled)
			patch->diffuse = params->diffuse;
		if(params->reflect_enabled)
			patch->reflect = params->reflect;
		if(params->transpar_enabled)
			patch->transpar = params->transpar;
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
	STARS_INFO *info;

	info = new STARS_INFO;
	if(info)
	{
		info->tinfo.name = "Stars";
		info->tinfo.dialog = dialog;
		info->tinfo.defaultdata = &default_data;
		info->tinfo.datasize = sizeof(DIALOG_DATA);
		info->tinfo.cleanup = texture_cleanup;
		info->tinfo.work = (void (*)(TEXTURE_INFO*, void*, TEXTURE_PATCH*, VECTOR*))texture_work;
	}

	return (TEXTURE_INFO*)info;
}

