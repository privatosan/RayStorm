/***************
 * PROGRAM:       Modeler
 * NAME:          cylinder.h
 * DESCRIPTION:   cylinder class definition
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    25.03.97 ah    initial release
 ***************/

#ifndef CYLINDER_H
#define CYLINDER_H

#ifndef VECMATH_H
#include "vecmath.h"
#endif

#ifndef OBJECT_H
#include "object.h"
#endif

class CYLINDER : public OBJECT
{
	public:
		CYLINDER();
		virtual void CalcBBox();
		virtual void Draw(DISPLAY*,MATRIX_STACK*);
		virtual void SetObject(DISPLAY*, VECTOR*, VECTOR*, VECTOR*, VECTOR*, VECTOR*);
		virtual BOOL WriteObject(struct IFFHandle *);
		virtual int GetType() { return OBJECT_CYLINDER; }
		virtual rsiResult ToRSI(rsiCONTEXT*, MATRIX_STACK*, void**);
		virtual OBJECT *DupObj();
		virtual int GetSize();
		virtual BOOL CSGAble() { return TRUE; }
		virtual BOOL IsVisible(DISPLAY*);
};

OBJECT *ParseCylinder(struct IFFHandle *, OBJECT *, int);

#endif /* CYLINDER_H */
