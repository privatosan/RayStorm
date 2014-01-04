/***************
 * PROGRAM:       Modeler
 * NAME:          mouse.h
 * DESCRIPTION:   MUI draw class
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    28.06.96 ah    initial release
 *    31.01.97 ah    added HandleMouseButtons()
 ***************/

#ifndef MOUSE_H
#define MOUSE_H

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef VECMATH_H
#include "vecmath.h"
#endif

#ifndef DISPLAY_H
#include "display.h"
#endif

#ifndef __AMIGA__
class CCamView;
#endif

// flags for HandleMouseButtons
#define BUTTON_NONE           0x00
#define BUTTON_LEFTDOWN       0x01
#define BUTTON_LEFTUP         0x02
#define BUTTON_RIGHTDOWN      0x04
#define BUTTON_RIGHTUP        0x08
#define BUTTON_LEFTSHIFT      0x10
#define BUTTON_RIGHTSHIFT     0x20

// minimum distance from lookpoint of virtual camera
#define MIN_DIST  1e-3f
// minimum zoom factor
#define MIN_ZOOM  1e-2f

typedef struct
{
	int dx,dy,dz;
	int oldmousex,oldmousey;
	VECTOR oldvalign;
	VECTOR oldvpos;
	float olddist;
	float oldzoom;
	VECTOR oldvp;
	UBYTE olddisplay;
	BOOL changed;
	DISPLAY *display;
	OBJECT *selobj;
#ifdef __AMIGA__
	Object *win;
	Object *obj;
	struct IClass *cl;
	struct NewMenu *popup_menu;
#else
	CCamView *pView;
#endif
} MOUSEDATA;

void DoMouseMove(DISPLAY*, int, int, MOUSEDATA*);
void HandleMouseButtons(MOUSEDATA*, int, int, int, BOOL*);

BOOL MouseTo3D(DISPLAY*, int, int, VECTOR*);
BOOL Center(DISPLAY*, int, int);
BOOL Find(VIEW*, OBJECT*);
BOOL Focus(VIEW*, BOOL);
void ZoomIn(VIEW*);
void ZoomOut(VIEW*);

#endif
