/***************
 * PROGRAM:       Modeler
 * NAME:          renderwindow_class.h
 * DESCRIPTION:   definitions for render window
 *	AUTHORS:			Andreas Heumann
 * HISTORY:
 *		DATE		NAME	COMMENT
 *		28.05.96	ah		initial release
 ***************/

#ifndef RENDERWINDOW_CLASS_H
#define RENDERWINDOW_CLASS_H

#ifndef MUIDEFS_H
#include "muidefs.h"
#endif

#ifndef DISPLAY_H
#include "display.h"
#endif

#ifndef CAMERA_H
#include "camera.h"
#endif

struct RenderWindow_Data
{
	Object *originator;
	Object *ns_xres, *ns_yres, *ns_xmin, *ns_ymin, *ns_xmax, *ns_ymax,
		*cm_enable, *lv_res, *cm_quick, *sl_depth, *ls_minobjects,
		*pf_name, *b_view, *cy_fmt, *cm_show, *b_ok, *b_cancel;
	CAMERA *camera;
	ULONG flags;

	char **resolutions;
	ULONG *res;
};

SAVEDS ASM ULONG RenderWindow_Dispatcher(REG(a0) struct IClass*,REG(a2) Object*,REG(a1) Msg);

#endif
