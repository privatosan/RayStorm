/***************
 * PROGRAM:       Modeler
 * NAME:          slider_class.h
 * DESCRIPTION:   definitions for slider subclass
 *	AUTHORS:			Andreas Heumann
 * HISTORY:
 *		DATE		NAME	COMMENT
 *		20.11.96	ah		initial release
 ***************/

#ifndef SLIDER_CLASS_H
#define SLIDER_CLASS_H

struct StringSlider_Data
{
	Object *sl_slider, *fs_string;
	int lower,upper;
	ULONG lowerbound,upperbound;
};

SAVEDS ASM ULONG StringSlider_Dispatcher(REG(a0) struct IClass*,REG(a2) Object*,REG(a1) Msg);

#endif
