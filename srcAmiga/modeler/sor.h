/***************
 * PROGRAM:       Modeler
 * NAME:          sor.h
 * DESCRIPTION:   This file includes the sor class definition
 * AUTHORS:       Andreas Heumann, Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    09.04.97 ah    initial release
 ***************/

#ifndef SOR_H
#define SOR_H

#ifndef VECMATH_H
#include "vecmath.h"
#endif

#ifndef OBJECT_H
#include "object.h"
#endif

#define SOR_NODE_FIRST           0x01     // first node
#define SOR_NODE_LAST            0x02     // last node
#define SOR_NODE_SPLINELAST      0x04     // last spline
#define SOR_NODE_NODESELECTED    0x08     // node is selected
#define SOR_NODE_SPLINESELECTED  0x10     // spline is selected because previous, next or this node is selected
														// valid for spline which is following this node
#define SOR_NODE_NODEDIRTY       0x20
#define SOR_NODE_SPLINEDIRTY     0x40
#define SOR_NODE_RECALC          0x80     // the coefficients a,b,c and d need to be recalculated

class SOR_NODE : public DLIST
{
	public:
		VECT2D p;
		float a,b,c,d;
		UWORD flags;

		SOR_NODE();
		void Limit(VECTOR*, MATRIX*, VECTOR*);
};

class SOR : public OBJECT
{
	private:
		SOR_NODE *nodes;

		void DrawIt(DISPLAY*,MATRIX_STACK*);

	public:
		SOR();
		~SOR();
		virtual rsiResult ToRSI(rsiCONTEXT*, MATRIX_STACK*, void**);
		virtual OBJECT *DupObj();
		virtual void Draw(DISPLAY*,MATRIX_STACK*);
		virtual void CalcBBox();
		virtual void SetObject(DISPLAY*,VECTOR*,VECTOR*,VECTOR*,VECTOR*,VECTOR*);
		virtual BOOL WriteObject(struct IFFHandle *);
		virtual int GetType() { return OBJECT_SOR; }
		virtual int GetSize();
		virtual BOOL CSGAble() { return TRUE; }
		virtual BOOL TestSelectedPoints(DISPLAY*,int,int,BOOL,MATRIX_STACK*);
		virtual BOOL AddPoint(VECTOR*);
		virtual BOOL IsVisible(DISPLAY*);
		SOR_NODE *AddNode(VECT2D*);
		void AddNode(SOR_NODE*);
		void RemoveNode(SOR_NODE*);
		SOR_NODE *RemoveNodeAtPos(VECT2D*);
		void FreeAllNodes();
	private:
		void DrawTrackPoints(DISPLAY*, MATRIX_STACK*);
		void DrawEdited(DISPLAY*, MATRIX_STACK*);
		BOOL DeleteSelectedPoints(UNDO_DELETE*);
};

OBJECT *ParseSOR(struct IFFHandle *, OBJECT *, int);

#endif /* SOR_H */
