/***************
 * MODUL:         checker
 * NAME:          checker.cpp
 * DESCRIPTION:   Checker Texture
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    29.03.98 ah    initial release
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
} DIALOG_DATA;

typedef struct
{
	TEXTURE_INFO tinfo;
} CHECKER_INFO;

DIALOG_DATA default_data =
{
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
static void SAVEDS texture_work(CHECKER_INFO *info, DIALOG_DATA *params, TEXTURE_PATCH *patch, VECTOR *v)
{
	// check for even numbers
	if ((int)((floor(v->x) + floor(v->y) + floor(v->z))) & 1)
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
	CHECKER_INFO *info;

	info = new CHECKER_INFO;
	if(info)
	{
		info->tinfo.name = "Checker";
		info->tinfo.dialog = dialog;
		info->tinfo.defaultdata = &default_data;
		info->tinfo.datasize = sizeof(DIALOG_DATA);
		info->tinfo.cleanup = texture_cleanup;
		info->tinfo.work = (void (*)(TEXTURE_INFO*, void*, TEXTURE_PATCH*, VECTOR*))texture_work;
	}

	return (TEXTURE_INFO*)info;
}

