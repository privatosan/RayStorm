/***************
 * MODULE:			coldef.h
 * PROJECT:			Modeler
 * VERSION:			1.0 20.02.1996
 * DESCRIPTION:	pen and brush definitions
 * AUTHORS:			Andreas Heumann, Mike Hesser
 * HISTORY:			DATE		NAME	COMMENT
 *						20.02.95	mh		initial release
 ***************/

#ifndef COLDEF_H
#define COLDEF_H

#include <afxwin.h>

#define MAX_PENS	6
#define MAX_BRUSHES 3

#define PEN_BACKGROUND	0
#define PEN_SELECTED		1
#define PEN_UNSELECTED	2
#define PEN_GRID			3
#define PEN_GRIDORTHO	4
#define PEN_BLACK			5
#define PEN_WHITE		1

#define BRUSH_BACKGROUND	0
#define BRUSH_SELECTED		1
#define BRUSH_UNSELECTED	2

#define RGB_BACKGROUND	0
#define RGB_SELECTED		1
#define RGB_UNSELECTED	2
#define RGB_GRID			3
#define RGB_BLACK			4
#define RGB_CLICKUNSEL	5

void InitColorObjects();
void DeleteColorObjects();
CPen *SetPen(CDC*, int);
CBrush *SetBrush(CDC*, int);
CBrush *GetBrush(int);
COLORREF GetRGB(int);
#endif
