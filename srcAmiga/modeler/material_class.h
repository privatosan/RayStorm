/***************
 * PROGRAM:       Modeler
 * NAME:          material_class.h
 * DESCRIPTION:   definitions for material subclass
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    02.05.97 ah    initial release
 ***************/

#ifndef MATERIAL_CLASS_H
#define MATERIAL_CLASS_H

#ifndef LIST_H
#include "list.h"
#endif

class RECALC_ITEM : public SLIST
{
	public:
		Object *obj;
};

struct Material_Data
{
	Object *contextmenu, *contextobject, *main;
	RECALC_ITEM *recalclist;
	struct SubTask *st;
	LONG sigbit;
	BOOL initchange;
};

SAVEDS ASM ULONG Material_Dispatcher(REG(a0) struct IClass*,REG(a2) Object*,REG(a1) Msg);

#endif
