/***************
 * PROGRAM:       Modeler
 * NAME:          list.h
 * DESCRIPTION:   list class definition
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    10.12.95 ah    initial release
 *    07.01.97 ah    added SetNext()
 *    11.04.97 ah    added DLIST
 *    15.05.97 ah    added InsertSorted()
 *    05.04.98 ah    added DLIST:Remove()
 ***************/

#ifndef LIST_H
#define LIST_H

#define INSERT_VERT  0
#define INSERT_HORIZ 1

class SLIST    // simple list
{
	protected:
		SLIST *next;

	public:
		SLIST();
		virtual ~SLIST() { };
		void Insert(SLIST **);
		void InsertSorted(SLIST **, int (*comparefunc)(void*,void*));
		void Append(SLIST **);
		void DeChain(SLIST **);
		void Remove(SLIST **);
		void RemoveEnd(SLIST **);
		virtual SLIST *GetNext() { return next; };
		void SetNext(SLIST *newnext) { next = newnext; };
		void FreeList();
};

class DLIST    // double linked list
{
	private:
		DLIST *prev, *next;

	public:
		DLIST();
		virtual ~DLIST() { };
		void Insert(DLIST**);
		void InsertAfter(DLIST*);
		void Append(DLIST**);
		void DeChain(DLIST**);
		void Remove(DLIST**);
		void FreeList();
		virtual DLIST *GetNext() { return next; };
		DLIST *GetPrev() { return prev; };
};

class LIST : public SLIST     // hierarchical list
{
	private:
		LIST *prev;
		LIST *up,*down;

	public:
		LIST();
		void Insert(LIST *,int);
		void Append(LIST **);
		void Remove(LIST **);
		void Delete(LIST **);
		void FreeList();
		void DeChain(LIST **);
		virtual LIST *GetPrev() { return prev; };
		LIST *GoUp() { return up; };
		LIST *GoDown() { return down; };
		void Init() { next = prev = up = down = NULL; };
		void InitWithoutDown() { next = prev = up = NULL; };
		void AddTo(LIST *);
};

#endif /* LIST_H */
