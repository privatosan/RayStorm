/***************
 * PROGRAM:       Modeler
 * NAME:          box.h
 * DESCRIPTION:   box class definition
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    11.01.97 ah    initial release
 ***************/

#ifndef BOX_H
#define BOX_H

#ifndef VECMATH_H
#include "vecmath.h"
#endif

#ifndef OBJECT_H
#include "object.h"
#endif

// Box
class BOX : public OBJECT
{
	public:
		BOX();
		virtual void Draw(DISPLAY*,MATRIX_STACK*);
		virtual void SetObject(DISPLAY*, VECTOR*, VECTOR*, VECTOR*, VECTOR*, VECTOR*);
		virtual BOOL WriteObject(struct IFFHandle *);
		virtual int GetType() { return OBJECT_BOX; }
		virtual rsiResult ToRSI(rsiCONTEXT *, MATRIX_STACK*, void**);
		virtual OBJECT *DupObj();
		virtual int GetSize();
		virtual BOOL CSGAble() { return TRUE; }
		virtual BOOL IsVisible(DISPLAY*);
};

OBJECT *ParseBox(struct IFFHandle *, OBJECT *, int);

#endif /* BOX_H */
