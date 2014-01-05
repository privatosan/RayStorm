/***************
 * NAME:          image.h
 * DESCRIPTION:   Definiions for image class
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    09.12.95 ah    initial release
 ***************/

#ifndef IMAGE_H
#define IMAGE_H

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef COLOR_H
#include "color.h"
#endif

class IMAGE
{
	public:
		SMALL_COLOR *colormap;
		ULONG width, height;

		IMAGE();
		~IMAGE();

		BOOL Load(char*, char*);
#ifdef __AMIGA__
		BOOL LoadAsDatatype(char*);
#endif
};

#endif // IMAGE_H
