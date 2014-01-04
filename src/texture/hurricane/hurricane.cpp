/***************
 * MODUL:         Hurricane
 * NAME:          hurrican.cpp
 * DESCRIPTION:   Hurricane texture
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    30.09.98 ah    initial release
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
			DIALOG_LABEL, DE("Radius"),
			DIALOG_FLOAT,
			DIALOG_LABEL, DE("Twist"),
			DIALOG_FLOAT,
			DIALOG_LABEL, DE("Offset"),
			DIALOG_FLOAT,
			DIALOG_LABEL, DE("Scale"),
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
	float max_radius;
	float twist;
	float offset;
	float scale;
} DIALOG_DATA;

typedef struct
{
	TEXTURE_INFO tinfo;
	short *hashTable;
} HURRICANE_INFO;

DIALOG_DATA default_data =
{
	{ .8f, .6f, .2f },
	TRUE,
	{ 0.f, 0.f, 0.f },
	FALSE,
	{ 0.f, 0.f, 0.f },
	FALSE,
	5,
	1.f,
	.5f,
	.5,
	.7f

};

/*************
 * DESCRIPTION:   This is the cleanup function for the textures.
 * INPUT:         tinfo
 * OUTPUT:        pointer to texture info, NULL if failed
 *************/
extern "C" void SAVEDS texture_cleanup(HURRICANE_INFO *tinfo)
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
extern "C" void SAVEDS texture_work(HURRICANE_INFO *info, DIALOG_DATA *params, TEXTURE_PATCH *patch, VECTOR *v)
{
	float radius, eye_weight, dist, angle, sine, cosine, val, one_minus_val;
	VECTOR p;

	eye_weight = 1.f;

	radius = sqrt(v->x*v->x + v->y*v->y);

	if(radius < params->max_radius)
	{
		// inside of hurricane
		dist = pow((params->max_radius - radius)/params->max_radius, 3);
		angle = params->twist * 2.f * PI * dist;
		sine = sin(angle);
		cosine = cos(angle);
		p.x = v->x * cosine - v->y * sine;
		p.y = v->x * sine + v->y * cosine;
		p.z = v->z;
		// subtract out "eye" of storm
		if(radius < 0.1f * params->max_radius)
		{
			// if in "eye"
			eye_weight = 1.f - (radius * 10.f / params->max_radius);   // normalize
			// invert and make it nonlinear
			eye_weight = pow(1.f - eye_weight, 4);
		}
		else
			eye_weight = 1.f;
	}
	else
	{
		p = *v;
		//eye_weight = params->max_radius / radius;
	}

	if(eye_weight > 0.f)
	{
		// if in "storm" area
		val = fBm(&p, 1.0f, 2.0f, params->octaves, info->hashTable);
		val = fabs(eye_weight * (params->offset + params->scale * val));
		one_minus_val = 1.f - val;

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
	HURRICANE_INFO *info;

	info = new HURRICANE_INFO;
	if(info)
	{
		if(!InitNoise(&info->hashTable))
		{
			delete info;
			return NULL;
		}

		info->tinfo.name = "Hurricane";
		info->tinfo.dialog = dialog;
		info->tinfo.defaultdata = &default_data;
		info->tinfo.datasize = sizeof(DIALOG_DATA);
		info->tinfo.cleanup = (void (*)(TEXTURE_INFO*))texture_cleanup;
		info->tinfo.work = (void (*)(TEXTURE_INFO*, void*, TEXTURE_PATCH*, VECTOR*))texture_work;
	}

	return (TEXTURE_INFO*)info;
}

