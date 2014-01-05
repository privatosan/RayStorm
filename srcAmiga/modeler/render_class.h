/***************
 * PROGRAM:       Modeler
 * NAME:          render_class.h
 * DESCRIPTION:   definitions for render class
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    02.11.96 ah    initial release
 ***************/

#ifndef RENDER_CLASS_H
#define RENDER_CLASS_H

#ifndef GRAPHICS_RASTPORT_H
#include <graphics/rastport.h>
#endif

#ifndef MUIDEFS_H
#include "muidefs.h"
#endif

struct Render_Data
{
	rsiSMALL_COLOR *data;
	int line, lines;
	int xres, yres;
	ULONG *colors;
	struct RastPort rp;
	rsiSMALL_COLOR *colormap;
	ColorMap *cmap;
};

SAVEDS ASM ULONG Render_Dispatcher(REG(a0) struct IClass*,REG(a2) Object*,REG(a1) Msg);

#endif
