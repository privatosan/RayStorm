/***************
 * PROGRAM:       Modeler
 * NAME:          color_class.h
 * VERSION:       1.0 09.02.1996
 * DESCRIPTION:   definitions for color window class
 *	AUTHORS:			Andreas Heumann
 * HISTORY:
 *		DATE		NAME	COMMENT
 *		09.02.96	ah		initial release
 ***************/

#ifndef COLOR_CLASS_H
#define COLOR_CLASS_H

struct Color_Data
{
	Object *originator;
	Object *cadj,*ns_red,*ns_green,*ns_blue;
	Object *b_ok,*b_cancel;
	Object *colorfield;
};

SAVEDS ASM ULONG Color_Dispatcher(REG(a0) struct IClass*,REG(a2) Object*,REG(a1) Msg);

#endif
