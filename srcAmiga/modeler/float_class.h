/***************
 * PROGRAM:       Modeler
 * NAME:          float_class.h
 * DESCRIPTION:   definitions for float subclass
 *	AUTHORS:			Andreas Heumann
 * HISTORY:
 *		DATE		NAME	COMMENT
 *		20.07.96	ah		initial release
 ***************/

#ifndef FLOAT_CLASS_H
#define FLOAT_CLASS_H

struct Float_Data
{
	Object *fs_gadget;
	int lower,upper;
	BOOL lowerbound,upperbound;
	BOOL lowerinside,upperinside;
	float value;
};

SAVEDS ASM ULONG Float_Dispatcher(REG(a0) struct IClass*,REG(a2) Object*,REG(a1) Msg);

#endif
