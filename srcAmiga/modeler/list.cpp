/***************
 * PROGRAM:       Modeler
 * NAME:          list.cpp
 * DESCRIPTION:   Functions for list class
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    10.12.95 ah    initial release
 *    20.01.97 ah    added SLIST:FreeList()
 *    11.04.97 ah    added DLIST
 *    15.05.97 ah    added InsertSorted()
 *    05.04.98 ah    added DLIST:Remove()
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
SLIST::SLIST()
{
	next = NULL;
}

/*************
 * DESCRIPTION:   Insert an element at the beginning of a simple list
 * INPUT:         root     root double pointer
 * OUTPUT:        none
 *************/
void SLIST::Insert(SLIST **root)
{
	next = *root;
	*root = this;
}

/*************
 * DESCRIPTION:   Insert an element sorted
 * INPUT:         root     root double pointer
 *                comparefunc    functions which is called to determinate sort
 *                It returns:
 *                > 0 -> a > b
 *                = 0 -> a = b
 *                < 0 -> a < b
 * OUTPUT:        none
 *************/
void SLIST::InsertSorted(SLIST **root, int (*comparefunc)(void*, void*))
{
	SLIST *cur, *prev;

	cur = *root;
	prev = NULL;
	if(cur)
	{
		while(comparefunc(cur, this) < 0)
		{
			prev = cur;
			cur = cur->next;
			if(!cur)
				break;
		}
	}

	if(!prev)
		this->Insert(root);
	else
	{
		prev->next = this;
		this->next = cur;
	}
}

/*************
 * DESCRIPTION:   Append an element to the end of a simple list
 * INPUT:         root     root double pointer
 * OUTPUT:        none
 *************/
void SLIST::Append(SLIST **root)
{
	SLIST *cur;

	// skip to end
	if(*root)
	{
		cur = *root;
		while(cur->GetNext())
			cur = cur->GetNext();
		cur->next = this;
		this->next = NULL;
	}
	else
		this->Insert(root);
}

/*************
 * DESCRIPTION:   Remove an element from a simple list without deleting it
 * INPUT:         root     root double pointer
 * OUTPUT:        none
 *************/
void SLIST::DeChain(SLIST **root)
{
	SLIST *cur,*prev;

	prev = NULL;
	cur = *root;
	while(cur && (cur != this))
	{
		prev = cur;
		cur = cur->GetNext();
	}
	if(cur)
	{
		if(prev)
			prev->next = cur->next;
		else
			*root = cur->next;
	}
}

/*************
 * DESCRIPTION:   Remove an element from a simple list
 * INPUT:         root     root double pointer
 * OUTPUT:        none
 *************/
void SLIST::Remove(SLIST **root)
{
	SLIST *cur,*prev;

	prev = NULL;
	cur = *root;
	while(cur && (cur != this))
	{
		prev = cur;
		cur = cur->GetNext();
	}
	if(cur)
	{
		if(prev)
			prev->next = cur->next;
		else
			*root = cur->next;
		delete cur;
	}
}

/*************
 * DESCRIPTION:   Remove an element from the end of a simple list
 * INPUT:         root     root double pointer
 * OUTPUT:        none
 *************/
void SLIST::RemoveEnd(SLIST **root)
{
	SLIST *cur;

	// skip to end
	if(*root)
	{
		cur = *root;
		while(cur->GetNext())
			cur = cur->GetNext();
		cur->Remove(root);
	}
	else
		this->Remove(root);
}

/*************
 * DESCRIPTION:   Remove one element and its childs
 * INPUT:         none
 * OUTPUT:        none
 *************/
void SLIST::FreeList()
{
	SLIST *cur, *next;

	cur = this;
	while(cur)
	{
		next = cur->GetNext();
		delete cur;
		cur = next;
	}
}

/*************
 * DESCRIPTION:   Constructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
DLIST::DLIST()
{
	next = prev = NULL;
}

/*************
 * DESCRIPTION:   Insert an element at the beginning of a double linked list
 * INPUT:         root     root double pointer
 * OUTPUT:        none
 *************/
void DLIST::Insert(DLIST **root)
{
	if(*root)
		(*root)->prev = this;
	next = *root;
	*root = this;
}

/*************
 * DESCRIPTION:   Insert an element after a given element of a double linked list
 * INPUT:         where    element to insert after
 * OUTPUT:        none
 *************/
void DLIST::InsertAfter(DLIST *where)
{
	next = where->next;
	if(next)
		next->prev = this;
	prev = where;
	where->next = this;
}

/*************
 * DESCRIPTION:   Append an element to the end of a double linked list
 * INPUT:         root     root double pointer
 * OUTPUT:        none
 *************/
void DLIST::Append(DLIST **root)
{
	DLIST *cur;

	// skip to end
	if(*root)
	{
		cur = *root;
		while(cur->GetNext())
			cur = cur->GetNext();
		cur->next = this;
		this->next = NULL;
		this->prev = cur;
	}
	else
		this->Insert(root);
}

/*************
 * DESCRIPTION:   Remove an element from a double linked list without deleting it
 * INPUT:         root     root double pointer
 * OUTPUT:        none
 *************/
void DLIST::DeChain(DLIST **root)
{
	if(prev)
		prev->next = next;
	if(next)
		next->prev = prev;

	if(this == *root)
		*root = this->next;
}

/*************
 * DESCRIPTION:   Remove an element from a double linked list
 * INPUT:         root     root double pointer
 * OUTPUT:        none
 *************/
void DLIST::Remove(DLIST **root)
{
	DeChain(root);
	delete this;
}

/*************
 * DESCRIPTION:   Remove one element and its childs
 * INPUT:         none
 * OUTPUT:        none
 *************/
void DLIST::FreeList()
{
	DLIST *cur, *next;

	cur = this;
	while(cur)
	{
		next = cur->GetNext();
		delete cur;
		cur = next;
	}
}

/*************
 * DESCRIPTION:   Constructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
LIST::LIST()
{
	prev = NULL;
	up = NULL;
	down = NULL;
}

/*************
 * DESCRIPTION:   Remove one element without destroying the list
 * INPUT:         root     root double pointer
 * OUTPUT:        none
 *************/
void LIST::Remove(LIST **root)
{
	LIST *list;

	if(down)
	{
		down->up = up;
		down->prev = prev;

		if(prev)
		{
			if(this == *root)
				*root = down;

			prev->next = (SLIST*)down;
		}

		if(up)
			up->down = down;

		list = down;
		while(list->next)
			list = (LIST*)(list->next);

		list->next = next;
		if(list->next)
			((LIST*)(list->next))->prev = list;
	}
	else
	{
		if(prev)
			prev->next = next;
		else
		{
			if(this == *root)
				*root = (LIST*)next;

			if(next && up)
			{
				((LIST*)next)->up = up;
				up->down = ((LIST*)next);
			}
			else
			{
				if(down)
				{
					down->up = up;
					up->down = down;
				}
				else
					up->down = NULL;
			}
		}

		if(next)
			((LIST*)next)->prev = prev;
	}
}

/*************
 * DESCRIPTION:   Delete one element and its childs without destroying the list
 * INPUT:         none
 * OUTPUT:        none
 *************/
void LIST::Delete(LIST **root)
{
	DeChain(root);

	if(down)
		down->FreeList();

	delete this;
}

/*************
 * DESCRIPTION:   remove one element and it's childs from list without destroying it
 * INPUT:         root     root double pointer
 * OUTPUT:        none
 *************/
void LIST::DeChain(LIST **root)
{
	if(this == *root)
	{
		*root = (LIST*)next;
		if(*root)
			(*root)->prev = NULL;
	}
	else
	{
		if(next)
		{
			((LIST*)next)->prev = prev;
			((LIST*)next)->up = up;

			if(up)
				up->down = (LIST*)next;
		}
		else
		{
			if(up)
				up->down = NULL;
		}
		if(prev)
			prev->next = next;
	}
}

/*************
 * DESCRIPTION:   Remove one element and its childs
 * INPUT:         none
 * OUTPUT:        none
 *************/
void LIST::FreeList()
{
	LIST *cur,*next,*down;

	cur = this;
	while(cur)
	{
		next = (LIST*)cur->next;
		down = cur->down;

		delete cur;

		if(down)
			down->FreeList();

		cur = next;
	}
}

/*************
 * DESCRIPTION:   Insert an element in list
 * INPUT:         where    where to insert element after
 *                dir      direction (1 = horizontal; 0 = vertical)
 * OUTPUT:        none
 *************/
void LIST::Insert(LIST *where,int dir)
{
	if(dir)
	{
		/* insert horizontal */
		next = where->next;
		where->next = this;
		prev = where;
		if(next)
			((LIST*)next)->prev = this;
	}
	else
	{
		/* insert vertical */
		next = where->down;
		if(next)
		{
			((LIST*)next)->prev = this;
			((LIST*)next)->up = NULL;
		}
		where->down = this;
		up = where;
	}
}

/*************
 * DESCRIPTION:   Append an element to a list
 * INPUT:         root     root pointer
 * OUTPUT:        none
 *************/
void LIST::Append(LIST **root)
{
	LIST *cur;

	if(!*root)
	{
		*root = this;
	}
	else
	{
		// skip to end
		cur = *root;
		while(cur->GetNext())
			cur = (LIST*)cur->GetNext();
		cur->next = this;
		prev = cur;
	}
}

/*************
 * DESCRIPTION:   Add an element to an object
 * INPUT:         where    where to insert element after
 * OUTPUT:        none
 *************/
void LIST::AddTo(LIST *where)
{
	if(where->down)
	{
		where->down->up = NULL;
		where->down->prev = this;
	}

	this->prev = NULL;
	this->next = where->down;
	where->down = this;
	this->up = where;
}
