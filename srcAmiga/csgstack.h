/***************
 * NAME:          csgstack.h
 * DESCRIPTION:   class definition for csgstack.cpp
 *	AUTHORS:			Andreas Heumann, Mike Hesser
 * HISTORY:
 *		DATE		NAME	COMMENT
 *		12.01.97	ah		initial release
 ***************/

#ifndef CSGSTACK_H
#define CSGSTACK_H

class CSGSTACK
{
	public:
		CSGSTACK *next;
		BOOL first;
		void *csg;

		CSGSTACK();
};

CSGSTACK *AddCSGItem(void*);
BOOL SetCSGObject(void*);
void DestroyCSGStack();

#endif
