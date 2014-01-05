/***************
 * MODUL:         list
 * NAME:          list.h
 * DESCRIPTION:   list class definition
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    11.02.95 ah    initial release
 *    09.12.96 ah    GetNext() is now an inline function
 *    21.07.97 ah    added Append()
 ***************/

#ifndef LIST_H
#define LIST_H

class LIST                 /* object */
{
	private:
		LIST *next;          /* next object */

	public:
		LIST();
		virtual ~LIST() { };
		void Insert(LIST**);
		void Append(LIST**);
		void Remove(LIST**);
		virtual LIST *GetNext() { return next; };
		void FreeList();
};

#endif /* LIST_H */
