/***************
 * PROGRAM:       Modeler
 * NAME:          numreq_class.h
 * DESCRIPTION:   definitions for numeric requester class
 *	AUTHORS:			Andreas Heumann
 * HISTORY:
 *		DATE		NAME	COMMENT
 *		02.04.96	ah		initial release
 ***************/

#ifndef NUMREQ_CLASS_H
#define NUMREQ_CLASS_H

struct NumReq_Data
{
	Object *originator;
	Object *t_num;
	Object *fs_num;
	Object *b_ok, *b_cancel;
	float value;
	char *text;
};

ULONG NumReq_New(struct IClass *,Object *,struct opSet *);
SAVEDS ASM ULONG NumReq_Dispatcher(REG(a0) struct IClass*,REG(a2) Object*,REG(a1) Msg);

#endif
