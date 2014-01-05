/***************
 * PROGRAM:       Modeler
 * NAME:          requview_class.h
 * DESCRIPTION:   definitions for requester view class
 *	AUTHORS:			Andreas Heumann
 * HISTORY:
 *		DATE		NAME	COMMENT
 *		05.11.96	ah		initial release
 ***************/

#ifndef REQUVIEW_CLASS_H
#define REQUVIEW_CLASS_H

#ifndef GRAPHICS_RASTPORT_H
#include <graphics/rastport.h>
#endif

#ifndef MUIDEFS_H
#include "muidefs.h"
#endif

#ifndef DISPLAY_H
#include "display.h"
#endif

#ifndef FLARES_H
#include "flares.h"
#endif

#ifndef STAR_H
#include "star.h"
#endif

struct RequView_Data
{
	int width, height;
	struct RastPort rp;
	DISPLAY *display;
	FLARE *flares;
	STAR *star;
	int flags;
};

SAVEDS ASM ULONG RequView_Dispatcher(REG(a0) struct IClass*,REG(a2) Object*,REG(a1) Msg);

#endif
