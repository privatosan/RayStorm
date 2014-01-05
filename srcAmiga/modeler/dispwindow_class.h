/***************
 * PROGRAM:       Modeler
 * NAME:          dispwindow_class.h
 * DESCRIPTION:   definitions for display window
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    02.09.96 ah    initial release
 ***************/

#ifndef DISPWINDOW_CLASS_H
#define DISPWINDOW_CLASS_H

#ifndef MUIDEFS_H
#include "muidefs.h"
#endif

struct DispWindow_Data
{
	Object *originator;
	Object *renderarea;
};

SAVEDS ASM ULONG DispWindow_Dispatcher(REG(a0) struct IClass*,REG(a2) Object*,REG(a1) Msg);

#endif
