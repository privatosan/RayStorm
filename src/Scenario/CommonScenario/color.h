/***************
 * PROGRAM:       Modeler
 * NAME:          color.h
 * DESCRIPTION:   definitions and structures for colors
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    27.01.96 ah    initial release
 ***************/

#ifndef COLOR_H
#define COLOR_H

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

typedef struct
{
	float r,g,b;
} COLOR;

typedef struct
{
	ULONG r,g,b;
} LCOLOR;

typedef struct
{
	UBYTE r,g,b, flags;
} SMALL_COLOR;

/*************
 * DESCRIPTION:   Sets the color
 * INPUT:         color    color
 *                r,g,b    values
 * OUTPUT:        -
 *************/
inline void SetColor(COLOR *color, float r, float g, float b)
{
	color->r = r;
	color->g = g;
	color->b = b;
}

// color definitions, COLOR_MAX must be the last entry
enum
{
	COLOR_AXIS,
	COLOR_BKGROUND,
	COLOR_BRUSH,
	COLOR_CAMERA,
	COLOR_GRID,
	COLOR_HOTSPOT,
	COLOR_UNSELECTED,
	COLOR_SELECTED,
	COLOR_SELBOX,
	COLOR_TEXT,
	COLOR_TEXTURE,
	COLOR_LIGHT,
	COLOR_MAX
};

char *GetColorString(ULONG);
void GetColorValue(ULONG, COLOR*);
void SetColorValue(ULONG, COLOR*);
void SetColorPrefs();
void GetColorPrefs();
void SetGLColor(int);

#endif /* COLOR_H */
