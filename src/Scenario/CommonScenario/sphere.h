/***************
 * PROGRAM:       Modeler
 * NAME:          sphere.h
 * DESCRIPTION:   sphere class definition
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    10.12.95 ah    initial release
 *    18.12.96 ah    added GetSize()
 ***************/

#ifndef SPHERE_H
#define SPHERE_H

#ifndef VECMATH_H
#include "vecmath.h"
#endif

#ifndef OBJECT_H
#include "object.h"
#endif

// Sphere
class SPHERE : public OBJECT
{
	public:
		float r;          // radius

		SPHERE();
		virtual void Draw(DISPLAY*,MATRIX_STACK*);
		virtual void SetObject(DISPLAY*, VECTOR*, VECTOR*, VECTOR*, VECTOR*, VECTOR*);
		virtual BOOL WriteObject(struct IFFHandle *);
		virtual int GetType() { return OBJECT_SPHERE; }
		virtual rsiResult ToRSI(rsiCONTEXT*, MATRIX_STACK*, void**);
		virtual OBJECT *DupObj();
		virtual int GetSize();
		virtual BOOL CSGAble() { return TRUE; }
		virtual void CalcBBox();
		virtual BOOL IsVisible(DISPLAY*);
};

OBJECT *ParseSphere(struct IFFHandle *, OBJECT *, int);

#endif /* SPHERE_H */
