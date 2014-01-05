/***************
 * PROGRAM:       Modeler
 * NAME:          cone.h
 * DESCRIPTION:   cone class definition
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    25.03.97 ah    initial release
 ***************/

#ifndef CONE_H
#define CONE_H

#ifndef VECMATH_H
#include "vecmath.h"
#endif

#ifndef OBJECT_H
#include "object.h"
#endif

class CONE : public OBJECT
{
	public:
		CONE();
		virtual void Draw(DISPLAY*,MATRIX_STACK*);
		virtual void SetObject(DISPLAY*, VECTOR*, VECTOR*, VECTOR*, VECTOR*, VECTOR*);
		virtual BOOL WriteObject(struct IFFHandle *);
		virtual int GetType() { return OBJECT_CONE; }
		virtual rsiResult ToRSI(rsiCONTEXT*, MATRIX_STACK*, void**);
		virtual OBJECT *DupObj();
		virtual int GetSize();
		virtual BOOL CSGAble() { return TRUE; }
		virtual void CalcBBox();
		virtual BOOL IsVisible(DISPLAY*);
};

OBJECT *ParseCone(struct IFFHandle *, OBJECT *, int);

#endif /* CONE_H */
