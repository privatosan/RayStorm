#ifndef PLUGIN_MOD_H
#define PLUGIN_MOD_H

#ifndef DIALOG_H
#include "dialog.h"
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
	UWORD p1,p2;
} PLUGIN_EDGE;

typedef struct
{
	UWORD e1,e2,e3;
} PLUGIN_TRIANGLE;

typedef struct _pinfo
{
	char *name;
	DIALOG_ELEMENT *dialog;
	void *defaultdata;
	ULONG datasize;
	void (*cleanup)(_pinfo *tinfo);
	void (*update)(_pinfo *info, void *data);
	void (*work)(_pinfo *info, void *data);

	void* (*MeshCreate)();
	void (*MeshPoints)(void *mesh, ULONG count, VECTOR *points);
	void (*MeshEdges)(void *mesh, ULONG count, PLUGIN_EDGE *edges);
	void (*MeshTriangles)(void *mesh, ULONG count, PLUGIN_TRIANGLE *triangles);
	ULONG (*DoMethod)(void *obj, ULONG method, ...);
} PLUGIN_INFO;

enum
{
	PLUG_IMAGE_DISPLAY,
};

#endif // PLUGIN_MOD_H
