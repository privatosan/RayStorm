/***************
 * PROGRAM:       Modeler
 * NAME:          Plugin_class.h
 * DESCRIPTION:   definitions for plugin window class
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    27.09.98 ah    initial release
 ***************/

#ifndef PLUGIN_CLASS_H
#define PLUGIN_CLASS_H

struct Plugin_Data
{
	Object *originator;
	Object *b_ok,*b_cancel;
};

SAVEDS ASM ULONG Plugin_Dispatcher(REG(a0) struct IClass*,REG(a2) Object*,REG(a1) Msg);

#endif
