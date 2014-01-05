#ifndef TEXTURE_MOD_H
#define TEXTURE_MOD_H

#ifndef DIALOG_H
#ifdef __STORM__
#include "rtxt:dialog.h"
#else
#include "dialog.h"
#endif
#endif

#ifndef COLOR_H
#include "color.h"
#endif

#ifndef VECMATH_H
#include "vecmath.h"
#endif

#ifndef SAVEDS
#ifdef __AMIGA__
#define SAVEDS __saveds
#else
#define SAVEDS
#endif // __AMIGA__
#endif

typedef struct
{
	COLOR ambient;          // ambient
	COLOR diffuse;          // diffuse
	COLOR specular;         // specular
	COLOR reflect;          // reflectivity
	COLOR transpar;         // transparency
	COLOR difftrans;        // Diffuse transmission 'curve'
	COLOR spectrans;        // Specular transmission 'curve'
	float refphong;         // reflected Phong coef
	float transphong;       // transmitted Phong coef
	float foglength;        // fog length
	float refrindex;        // index of refr
	float translucency;     // translucency
	VECTOR norm;           // surface normal
} TEXTURE_PATCH;

typedef struct _tinfo
{
	char *name;
	DIALOG_ELEMENT *dialog;
	void *defaultdata;
	ULONG datasize;
	void (*cleanup)(_tinfo *tinfo);
	void (*work)(_tinfo *info, void *params, TEXTURE_PATCH *patch, VECTOR *v);
} TEXTURE_INFO;

#endif // TEXTURE_MOD_H
