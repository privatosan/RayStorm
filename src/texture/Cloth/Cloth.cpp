/***************
 * MODUL:         Cloth
 * NAME:          cloth.cpp
 * DESCRIPTION:   Cloth hyper texture
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
			DIALOG_LABEL, DE("Bias"),
			DIALOG_FLOAT,
			DIALOG_LABEL, DE("Gain"),
			DIALOG_FLOAT,
		DIALOG_END,
	DIALOG_END,
	DIALOG_FINISH
};

typedef struct
{
	float bias, gain;
} DIALOG_DATA;

typedef struct
{
	TEXTURE_INFO tinfo;
} CLOTH_INFO;

DIALOG_DATA default_data =
{
	.3f,
	.9f
};

#define REC_LOG_HALF -1.442695041f

/*************
 * FUNCTION:      bias
 * DESCRIPTION:
 * INPUT:         a     value
 *                b     factor
 * OUTPUT:        new value
 *************/
inline float bias(float a, float b)
{
	return pow((double)a, log(b)*REC_LOG_HALF);
}

/*************
 * FUNCTION:      gain
 * DESCRIPTION:
 * INPUT:         a     value
 *                b     factor
 * OUTPUT:        new value
 *************/
inline float gain(float a, float b)
{
	float p = log(1. - b) * REC_LOG_HALF;

	if (a < .001f)
		return 0.f;
	else if (a > .999f)
		return 1.f;
	if (a < 0.5f)
		return pow((double)(2.f * a), (double)p) * 0.5f;
	else
		return 1.f - pow((double)(2.f * (1.f - a)), (double)p) * 0.5f;
}

/*************
 * FUNCTION:      Warp
 * DESCRIPTION:   this function makes the single threads
 * INPUT:         pos   position
 * OUTPUT:        density
 *************/
inline float Warp(VECTOR *pos, DIALOG_DATA *params)
{
	float z;
	float density;

	if(fabs(pos->z) > PI)
		return 0.f;

	z = pos->z + PI_2 * cos(pos->x) * cos(pos->y);
	if(fabs(z) > PI_2)
		return 0.f;

	density = cos(z) * cos(pos->y);
	density *= density;
	density = bias(density, params->bias);
	density = gain(density, params->gain);
	return density;
}

/*************
 * DESCRIPTION:   This is the cleanup function for the textures.
 * INPUT:         tinfo
 * OUTPUT:        pointer to texture info, NULL if failed
 *************/
static void SAVEDS texture_cleanup(CLOTH_INFO *tinfo)
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
static void SAVEDS texture_work(CLOTH_INFO *info, DIALOG_DATA *params, TEXTURE_PATCH *patch, VECTOR *v)
{
	VECTOR p;

	SetVector(&p, v->x, v->y, v->z);
	patch->density = Warp(&p, params);
	SetVector(&p, v->y, v->x, -v->z);
	patch->density += Warp(&p, params);
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
	CLOTH_INFO *info;

	info = new CLOTH_INFO;
	if(info)
	{
		info->tinfo.name = "Cloth";
		info->tinfo.dialog = dialog;
		info->tinfo.defaultdata = &default_data;
		info->tinfo.datasize = sizeof(DIALOG_DATA);
		info->tinfo.flags = FLAG_HYPER_SHADED;
		info->tinfo.cleanup = (void (*)(TEXTURE_INFO*))texture_cleanup;
		info->tinfo.work = (void (*)(TEXTURE_INFO*, void*, TEXTURE_PATCH*, VECTOR*))texture_work;
	}

	return (TEXTURE_INFO*)info;
}

