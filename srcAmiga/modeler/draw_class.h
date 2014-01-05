/***************
 * PROGRAM:       Modeler
 * NAME:          draw_class.h
 * DESCRIPTION:   MUI draw class definitions
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    10.12.95 ah    initial release
 ***************/

#ifndef DRAW_CLASS_H
#define DRAW_CLASS_H

#include <devices/input.h>

#ifndef MUIDEFS_H
#include "muidefs.h"
#endif

#ifndef DISPLAY_H
#include "display.h"
#endif

struct Draw_Data
{
	Object *win;
	Object *contextmenu;
	DISPLAY *display;
	int width, height;
	struct IOStdReq *InputIO;
	struct MsgPort *InputMP;
	struct InputEvent *FakeEvent;
	struct IEPointerPixel *NeoPix;
	int oldmousex, oldmousey;
};

SAVEDS ASM ULONG Draw_Dispatcher(REG(a0) struct IClass*, REG(a2) Object*, REG(a1) Msg);

void SetCoordDisplay(const VECTOR *, Object *);
void SetActiveGadget(Object*, int);
void Redraw(struct Draw_Data*, int, BOOL);
BOOL TestBreaked();

#endif
