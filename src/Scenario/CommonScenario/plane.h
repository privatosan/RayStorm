/***************
 * PROGRAM:       Modeler
 * NAME:          plane.h
 * DESCRIPTION:   plane class definition
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    03.06.96 ah    initial release
 *    18.12.96 ah    added GetSize()
 ***************/

#ifndef PLANE_H
#define PLANE_H

#ifndef VECMATH_H
#include "vecmath.h"
#endif

#ifndef OBJECT_H
#include "object.h"
#endif

// Plane
class PLANE : public OBJECT
{
	public:

		PLANE();
		virtual void Draw(DISPLAY*,MATRIX_STACK*);
		virtual void SetObject(DISPLAY*, VECTOR*, VECTOR*, VECTOR*, VECTOR*, VECTOR*);
		virtual BOOL WriteObject(struct IFFHandle *);
		virtual int GetType() { return OBJECT_PLANE; }
		virtual rsiResult ToRSI(rsiCONTEXT*, MATRIX_STACK*, void**);
		virtual OBJECT *DupObj();
		virtual int GetSize();
		virtual void CalcBBox();
		virtual BOOL IsVisible(DISPLAY*);
};

OBJECT *ParsePlane(struct IFFHandle *, OBJECT *, int);

#endif /* PLANE_H */
