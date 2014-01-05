/***************
 * PROGRAM:       Modeler
 * NAME:          prefswindow_class.h
 * DESCRIPTION:   definition for preference window class
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    10.02.96 ah    initial release
 *    06.03.97 ah    added external toolbar images flag
 *    07.12.97 ah    added light path
 ***************/

#ifndef PREFSWINDOW_CLASS_H
#define PREFSWINDOW_CLASS_H

#ifndef DISPLAY_H
#include "display.h"
#endif

struct PrefsWindow_Data
{
	Object *originator;
	Object *drawarea;
	Object *pf_project, *pf_object, *pf_texture, *pf_brush;
	Object *pf_material, *pf_light, *pf_view;
	Object *cm_showcoord, *cm_extimages;
	Object *cm_directrender;
	Object *ls_undomem, *ls_jpegquality, *ls_renderprio, *ls_renderstack;
	Object *lv_colors, *cm_usesurfacecolors, *coloradjust;
	Object *b_ok, *b_cancel;
	DISPLAY *disp;
};

SAVEDS ASM ULONG PrefsWindow_Dispatcher(REG(a0) struct IClass*,REG(a2) Object*,REG(a1) Msg);

#endif
