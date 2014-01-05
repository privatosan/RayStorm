/***************
 * PROGRAM:       Modeler
 * NAME:          animtree_class.h
 * DESCRIPTION:   definitions for anim list tree class
 *	AUTHORS:			Andreas Heumann
 * HISTORY:
 *		DATE		NAME	COMMENT
 *		25.10.97	ah		initial release
 ***************/

#ifndef ANIMTREE_CLASS_H
#define ANIMTREE_CLASS_H

#include <libraries/listtree_mcc.h>

struct AnimTree_Data
{
	Object *obj;
};

SAVEDS ASM ULONG AnimTree_Dispatcher(REG(a0) struct IClass*,REG(a2) Object*,REG(a1) Msg);

#endif
