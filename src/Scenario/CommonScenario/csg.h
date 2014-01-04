/***************
 * PROGRAM:       Modeler
 * NAME:          csg.h
 * DESCRIPTION:   csg class definition
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    04.04.97 ah    initial release
 ***************/

#ifndef CSG_H
#define CSG_H

#ifndef VECMATH_H
#include "vecmath.h"
#endif

#ifndef OBJECT_H
#include "object.h"
#endif

enum
{
	CSG_UNION,
	CSG_INTERSECTION,
	CSG_DIFFERENCE
};

class CSG : public OBJECT
{
	public:
		UWORD operation;

		CSG();
		virtual void Draw(DISPLAY*,MATRIX_STACK*);
		virtual void SetObject(DISPLAY*, VECTOR*, VECTOR*, VECTOR*, VECTOR*, VECTOR*);
		virtual BOOL WriteObject(struct IFFHandle *);
		virtual int GetType() { return OBJECT_CSG; }
		virtual rsiResult ToRSI(rsiCONTEXT*, MATRIX_STACK*, void**);
		virtual OBJECT *DupObj();
		virtual int GetSize();
		virtual BOOL CSGAble() { return TRUE; }
		virtual void CalcBBox();
		virtual BOOL IsVisible(DISPLAY*);
};

OBJECT *ParseCSG(struct IFFHandle *, OBJECT *, int);

#endif /* CSG_H */
