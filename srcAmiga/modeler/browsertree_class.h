/***************
 * PROGRAM:       Modeler
 * NAME:          browsertree_class.h
 * DESCRIPTION:   definitions for browser window class
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    22.03.97 ah    initial release
 ***************/

#ifndef BROWSERTREE_CLASS_H
#define BROWSERTREE_CLASS_H

#include <libraries/listtree_mcc.h>

#define IMAGE_AMOUNT 14

struct BrowserTree_Data
{
	Object *obj;
	BOOL redraw;
	Object *draw_area;
	Object *bimage[IMAGE_AMOUNT];
	Object *image[IMAGE_AMOUNT];
	struct MUIS_Listtree_TreeNode *drop;
	UWORD drop_flags;
	LONG old_entry;
	int mode;
	struct MUIS_Listtree_TreeNode *where;
	BOOL samelevel;
	BOOL dontselect;
	Object *contextmenu;
};

enum
{
	BROWSER_ALL,
	BROWSER_CAMERAS,
	BROWSER_SELECTEDSURFACE
};

SAVEDS ASM ULONG BrowserTree_Dispatcher(REG(a0) struct IClass*,REG(a2) Object*,REG(a1) Msg);

#endif
