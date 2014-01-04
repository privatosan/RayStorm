/***************
 * NAME:          itemlist.cpp
 * VERSION:       1.0 17.06.1996
 * DESCRIPTION:   Here are the routines to handle surface and actor
 *		names. (RayStorm itself only uses pointers not names, the
 *		interface has to handle the names)
 *	AUTHORS:			Andreas Heumann, Mike Hesser
 * HISTORY:
 *		DATE		NAME	COMMENT
 *		17.06.96	ah		initial release
 ***************/

#include <string.h>

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef ITEMLIST_H
#include "itemlist.h"
#endif

static ITEMLIST *surflist = NULL;
static ITEMLIST *actorlist = NULL;

/*************
 * FUNCTION:		ITEMLIST::ITEMLIST
 * DESCRIPTION:	Constructor
 * INPUT:			none
 * OUTPUT:			none
 *************/
ITEMLIST::ITEMLIST()
{
	name = NULL;
	item = NULL;
	next = NULL;
}

/*************
 * FUNCTION:		ITEMLIST::~ITEMLIST
 * DESCRIPTION:	Destructor
 * INPUT:			none
 * OUTPUT:			none
 *************/
ITEMLIST::~ITEMLIST()
{
	if(name)
		delete name;
}

/*************
 * FUNCTION:		FindSurfListItem
 * DESCRIPTION:	Search a surface by it's name and return
 *		it's address
 * INPUT:			name		name of surface
 * OUTPUT:			address of surface
 *************/
void *FindSurfListItem(char *name)
{
	ITEMLIST *item;

	item = surflist;
	while(item)
	{
		if(!strcmp(item->name,name))
			return item->item;
		item = item->next;
	}
	return NULL;
}

/*************
 * FUNCTION:		FindActorListItem
 * DESCRIPTION:	Search a actor by it's name and return
 *		it's address
 * INPUT:			name		name of actor
 * OUTPUT:			address of actor
 *************/
void *FindActorListItem(char *name)
{
	ITEMLIST *item;

	item = actorlist;
	while(item)
	{
		if(!strcmp(item->name,name))
			return item->item;
		item = item->next;
	}
	return NULL;
}

/*************
 * FUNCTION:		AddSurfList
 * DESCRIPTION:	Adds a surface to the list
 * INPUT:			name		name of surface
 *						adr		address of surface
 * OUTPUT:			created item or NULL if failed
 *************/
ITEMLIST *AddSurfList(char *name, void *adr)
{
	ITEMLIST *surf;

	surf = new ITEMLIST;
	if(!surf)
		return NULL;

	surf->name = new char[strlen(name)+1];
	if(!surf->name)
	{
		delete surf;
		return NULL;
	}
	strcpy(surf->name,name);

	surf->item = adr;

	surf->next = surflist;
	surflist = surf;

	return surf;
}

/*************
 * FUNCTION:		AddActorList
 * DESCRIPTION:	Adds a actor to the list
 * INPUT:			name		name of actor
 *						adr		address of actor
 * OUTPUT:			created item or NULL if failed
 *************/
ITEMLIST *AddActorList(char *name, void *adr)
{
	ITEMLIST *actor;

	actor = new ITEMLIST;
	if(!actor)
		return NULL;

	actor->name = new char[strlen(name)+1];
	if(!actor->name)
	{
		delete actor;
		return NULL;
	}
	strcpy(actor->name,name);

	actor->item = adr;

	actor->next = actorlist;
	actorlist = actor;

	return actor;
}

/*************
 * FUNCTION:		DestroyLists
 * DESCRIPTION:	Free the lists
 * INPUT:			none
 * OUTPUT:			none
 *************/
void DestroyLists()
{
	ITEMLIST *item,*next;

	item = surflist;
	while(item)
	{
		next = item->next;
		delete item;
		item = next;
	}
	surflist = NULL;

	item = actorlist;
	while(item)
	{
		next = item->next;
		delete item;
		item = next;
	}
	actorlist = NULL;
}

