/***************
 * PROGRAM:       Modeler
 * NAME:          statewindow_class.h
 * DESCRIPTION:   definitions for state window
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    16.07.96 ah    initial release
 *    25.02.97 ah    added t_testimated
 ***************/

#ifndef STATEWINDOW_CLASS_H
#define STATEWINDOW_CLASS_H

#ifndef MUIDEFS_H
#include "muidefs.h"
#endif

#ifndef DISPLAY_H
#include "display.h"
#endif

#ifndef SUBTASK_H
#include "subtask.h"
#endif

struct StateWindow_Data
{
	Object *originator;
	DISPLAY *display;
	Object *lv_log,*g_perc,*t_tspend,*t_testimated,*t_tleft,*b_ok,*b_show,*b_cancel;
	Object *dispwin;
	LONG sigbit;
	struct SubTask *st;
	struct RenderData renderdata;
};

SAVEDS ASM ULONG StateWindow_Dispatcher(REG(a0) struct IClass*,REG(a2) Object*,REG(a1) Msg);

#endif
