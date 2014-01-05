/***************
 * MODUL:         list
 * NAME:          list.cpp
 * DESCRIPTION:   Functions for list class
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    11.02.95 ah    initial release
 *    21.07.97 ah    added Append()
 ***************/

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef LIST_H
#include "list.h"
#endif

/*************
 * DESCRIPTION:   Constructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
LIST::LIST()
{
	next = NULL;
}

/*************
 * DESCRIPTION:   Insert a element to list
 * INPUT:         root     doublepointer to root
 * OUTPUT:        none
 *************/
void LIST::Insert(LIST **root)
{
	next = *root;
	*root = this;
}

/*************
 * DESCRIPTION:   Append a element to list
 * INPUT:         root     doublepointer to root
 * OUTPUT:        none
 *************/
void LIST::Append(LIST **root)
{
	LIST *cur;

	if(*root)
	{
		cur = *root;
		while(cur->next)
			cur = cur->next;
		cur->next = this;
	}
	else
	{
		Insert(root);
	}
}

/*************
 * DESCRIPTION:   remove a element from the list
 * INPUT:         root     doublepointer to root
 * OUTPUT:        none
 *************/
void LIST::Remove(LIST **root)
{
	LIST *cur, *prev=NULL;

	cur = *root;
	while(cur)
	{
		if(cur == this)
		{
			if(prev)
				prev->next = (LIST*)cur->GetNext();
			else
				*root = (LIST*)cur->GetNext();
			return;
		}
		prev = cur;
		cur = (LIST*)cur->GetNext();
	}
}

/*************
 * DESCRIPTION:   Free whole list
 * INPUT:         none
 * OUTPUT:        none
 *************/
void LIST::FreeList()
{
	LIST *next, *cur;

	cur = this;
	while(cur)
	{
		next = cur->GetNext();
		delete cur;
		cur = next;
	}
}
