/***************
 * NAME:          loader.h
 * DESCRIPTION:   Definiions for image class
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    21.08.98 ah    initial release
 ***************/

#ifndef IMAGE_H
#define IMAGE_H

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef COLOR_H
#include "color.h"
#endif

typedef struct
{
	SMALL_COLOR *colormap;
	ULONG width, height;
} IMAGE;

void Cleanup(IMAGE*);
BOOL Load(char *name, IMAGE*);

#endif // IMAGE_H
