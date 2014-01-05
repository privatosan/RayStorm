/***************
 * PROGRAM:       Modeler
 * NAME:          browser_class.h
 * DESCRIPTION:   definitions for browser window class
 *	AUTHORS:			Andreas Heumann
 * HISTORY:
 *		DATE		NAME	COMMENT
 *		08.07.96	ah		initial release
 ***************/

#ifndef BROWSER_CLASS_H
#define BROWSER_CLASS_H

#ifndef BROWSERTREE_CLASS_H
#include "browsertree_class.h"
#endif

struct Browser_Data
{
	Object *originator;
	Object *lv_list, *lv_listtree;
	Object *b_ok,*b_show,*b_cancel;
	int mode;
	BOOL dontselect, redraw;
	Object *draw_area;
	BOOL ok;
};

SAVEDS ASM ULONG Browser_Dispatcher(REG(a0) struct IClass*,REG(a2) Object*,REG(a1) Msg);

#endif
