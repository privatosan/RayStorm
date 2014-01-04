/***************
 * NAME:          itemlist.h
 * VERSION:       1.0 17.06.1996
 * DESCRIPTION:   class definition for itemlist.cpp
 *	AUTHORS:			Andreas Heumann, Mike Hesser
 * HISTORY:
 *		DATE		NAME	COMMENT
 *		17.06.96	ah		initial release
 ***************/

#ifndef ITEMLIST_H
#define ITEMLIST_H

class ITEMLIST
{
	public:
		ITEMLIST *next;
		char *name;
		void *item;

		ITEMLIST();
		~ITEMLIST();
};

void *FindSurfListItem(char*);
void *FindActorListItem(char*);
ITEMLIST *AddSurfList(char *, void *);
ITEMLIST *AddActorList(char *, void *);
void DestroyLists();

#endif
