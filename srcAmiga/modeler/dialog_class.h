/***************
 * PROGRAM:       Modeler
 * NAME:          dialog_class.h
 * DESCRIPTION:   definitions for RayStorm dialog subclass
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    26.05.98 ah    initial release
 ***************/

#ifndef DIALOG_CLASS_H
#define DIALOG_CLASS_H

#ifndef DIALOG_H
#include "dialog.h"
#endif // DIALOG_H

struct Dialog_Data
{
	DIALOG_ELEMENT *dialog;
	void *data;
};

SAVEDS ASM ULONG Dialog_Dispatcher(REG(a0) struct IClass*,REG(a2) Object*,REG(a1) Msg);

#endif // DIALOG_CLASS
