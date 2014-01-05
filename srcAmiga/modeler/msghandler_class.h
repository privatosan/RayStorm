/***************
 * PROGRAM:       Modeler
 * NAME:          msghandler_class.h
 * DESCRIPTION:   definitions for message handler subclass
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    30.08.96 ah    initial release
 ***************/

#ifndef MSGHANDLER_CLASS_H
#define MSGHANDLER_CLASS_H

class VALID_OBJECT
{
	public:
		VALID_OBJECT *next;
		Object *object;

		void Insert(VALID_OBJECT **);
		void Remove(VALID_OBJECT **);
		virtual VALID_OBJECT *GetNext() { return next; };
		void FreeList();
};

struct MsgHandler_Data
{
	VALID_OBJECT *objects;
};

SAVEDS ASM ULONG MsgHandler_Dispatcher(REG(a0) struct IClass*, REG(a2) Object*, REG(a1) Msg);

extern Object *msghandler;

#endif
