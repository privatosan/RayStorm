/***************
 * PROGRAM:       Modeler
 * NAME:          statistics_class.h
 * DESCRIPTION:   definitions for statistic window class
 *	AUTHORS:			Andreas Heumann
 * HISTORY:
 *		DATE		NAME	COMMENT
 *		05.12.96	ah		initial release
 ***************/

#ifndef STATISTICS_CLASS_H
#define STATISTICS_CLASS_H

struct Statistics_Data
{
	Object *originator;
	Object *lv_stats, *b_ok;
};

SAVEDS ASM ULONG Statistics_Dispatcher(REG(a0) struct IClass*,REG(a2) Object*,REG(a1) Msg);

#endif
