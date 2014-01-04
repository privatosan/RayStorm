/***************
 * NAME:          csgstack.cpp
 * DESCRIPTION:   Here are the routines to handle the CSG stack
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    12.01.97 ah    initial release
 ***************/

#include <string.h>

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef CSGSTACK_H
#include "csgstack.h"
#endif

#ifdef DEBUG_LIB
extern "C"
{
#endif
#ifndef RSI_H
#include "rsi.h"
#endif
#ifdef DEBUG_LIB
}
#endif
#include "rsi.h"
#ifdef __AMIGA__
#ifndef DEBUG_LIB
#ifndef RSI_LIB_H
#include "rsi_lib.h"
#endif
#endif
#endif

static CSGSTACK *root = NULL;
extern rsiCONTEXT *rc;

#ifdef __PPC__
extern Library *RayStormBase;
#endif

/*************
 * FUNCTION:      CSGSTACK::CSGSTACK
 * DESCRIPTION:   Constructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
CSGSTACK::CSGSTACK()
{
	first = TRUE;
	next = NULL;
}

/*************
 * FUNCTION:      AddCSGItem
 * DESCRIPTION:   Adds a CSG object to the stack
 * INPUT:         csg      CSG object
 * OUTPUT:        created item or NULL if failed
 *************/
CSGSTACK *AddCSGItem(void *csg)
{
	CSGSTACK *item;

	item = new CSGSTACK;
	if(!item)
		return NULL;

	item->csg = csg;

	item->next = root;
	root = item;

	return item;
}

/*************
 * FUNCTION:      SetCSGObject
 * DESCRIPTION:   Set one object for a CSG object
 * INPUT:         obj      object
 * OUTPUT:        TRUE if ok else FALSE
 *************/
BOOL SetCSGObject(void *obj)
{
	CSGSTACK *next;
	rsiResult err;

	if(!root)
		return FALSE;

	if(root->first)
	{
#ifdef __PPC__
		err = rsiSetCSG(RayStormBase, rc, root->csg, rsiTCSGObject1, obj, rsiTDone);
#else
		err = rsiSetCSG(rc, root->csg, rsiTCSGObject1, obj, rsiTDone);
#endif
		if(err)
			return FALSE;

		root->first = FALSE;
	}
	else
	{
#ifdef __PPC__
		err = rsiSetCSG(RayStormBase, rc, root->csg, rsiTCSGObject2, obj, rsiTDone);
#else
		err = rsiSetCSG(rc, root->csg, rsiTCSGObject2, obj, rsiTDone);
#endif
		if(err)
			return FALSE;

		next = root->next;
		delete root;
		root = next;
	}

	return TRUE;
}

/*************
 * FUNCTION:      DestroyCSGStack
 * DESCRIPTION:   Free the CSG stack
 * INPUT:         none
 * OUTPUT:        none
 *************/
void DestroyCSGStack()
{
	CSGSTACK *item,*next;

	item = root;
	while(item)
	{
		next = item->next;
		delete item;
		item = next;
	}
	root = NULL;
}
