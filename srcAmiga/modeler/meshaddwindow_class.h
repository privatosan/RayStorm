/***************
 * PROGRAM:       Modeler
 * NAME:          meshaddwindow_class.h
 * DESCRIPTION:   mesh add window class definition
 *	AUTHORS:			Andreas Heumann
 * HISTORY:
 *		DATE		NAME	COMMENT
 *		21.09.96	ah		initial release
 ***************/

#ifndef MESHADDWINDOW_CLASS_H
#define MESHADDWINDOW_CLASS_H

#ifndef MUIDEFS_H
#include "MUIdefs.h"
#endif

// mesh add window
struct MeshAddWindow_Data
{
	int type;
	Object *originator;
	Object *fs_vect1x, *fs_vect1y, *fs_vect1z;
	Object *fs_float1, *fs_float2, *ns_int1, *ns_int2;
	Object *cm_flag1, *cm_flag2;
	Object *b_ok, *b_cancel;
};

enum
{
	MESHADD_CUBE, MESHADD_SPHERE, MESHADD_PLANE, MESHADD_TUBE, MESHADD_CONE, MESHADD_TORUS
};

SAVEDS ASM ULONG MeshAddWindow_Dispatcher(REG(a0) struct IClass*,REG(a2) Object*,REG(a1) Msg);

#endif /* MESHADDWINDOW_CLASS_H */
