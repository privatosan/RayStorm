/***************
 * PROGRAM:       Modeler
 * NAME:          globalwindow_class.h
 * DESCRIPTION:   definitions for global attribute window
 *	AUTHORS:			Andreas Heumann
 * HISTORY:
 *		DATE		NAME	COMMENT
 *		25.04.96	ah		initial release
 ***************/

#ifndef GLOBALWINDOW_CLASS_H
#define GLOBALWINDOW_CLASS_H

struct GlobalWindow_Data
{
	Object *originator;
	Object *cf_backcol,
		*cm_backenable,
		*pf_backpic,
		*cm_reflenable,
		*pf_reflmap,
		*cf_ambient,
		*cf_fog,
		*fs_flen,
		*fs_fheight,
		*cy_antialias,
		*cf_anticont,
		*fs_gauss,
		*cy_distrib,
		*cy_softshad,
		*cm_randjit,
		*ns_xres,
		*ns_yres,
		*lv_res,
		*b_ok,
		*b_cancel,
		*b_viewbd,
		*b_viewrm;
};

SAVEDS ASM ULONG GlobalWindow_Dispatcher(REG(a0) struct IClass*,REG(a2) Object*,REG(a1) Msg);

#endif
