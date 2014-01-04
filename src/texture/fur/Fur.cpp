/***************
 * MODUL:         Fur
 * NAME:          fur.cpp
 * DESCRIPTION:   Fur hyper texture
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
		DIALOG_COLGROUP, 2,
			DIALOG_LABEL, DE("Frequency"),
			DIALOG_FLOAT,
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
	float frequency;
	float bias;
	float gain;
} DIALOG_DATA;

typedef struct
{
	TEXTURE_INFO tinfo;
	short *hashTable;
} FUR_INFO;

DIALOG_DATA default_data =
{
	20.f,
	.3f,
	.95f
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
 * DESCRIPTION:   This is the cleanup function for the textures.
 * INPUT:         tinfo
 * OUTPUT:        pointer to texture info, NULL if failed
 *************/
static void SAVEDS texture_cleanup(FUR_INFO *tinfo)
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
static void SAVEDS texture_work(FUR_INFO *info, DIALOG_DATA *params, TEXTURE_PATCH *patch, VECTOR *v)
{
	VECTOR p = *v;
	float density;

	VecNormalize(&p);
	VecScale(params->frequency, &p, &p);
	density = Noise(&p, info->hashTable);

	density = bias(density, params->bias);
	patch->density = gain(density, params->gain);
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
	FUR_INFO *info;

	info = new FUR_INFO;
	if(info)
	{
		if(!InitNoise(&info->hashTable))
		{
			delete info;
			return NULL;
		}

		info->tinfo.name = "Fur";
		info->tinfo.dialog = dialog;
		info->tinfo.defaultdata = &default_data;
		info->tinfo.datasize = sizeof(DIALOG_DATA);
		info->tinfo.flags = FLAG_HYPER_SHADED;
		info->tinfo.cleanup = (void (*)(TEXTURE_INFO*))texture_cleanup;
		info->tinfo.work = (void (*)(TEXTURE_INFO*, void*, TEXTURE_PATCH*, VECTOR*))texture_work;
	}

	return (TEXTURE_INFO*)info;
}

