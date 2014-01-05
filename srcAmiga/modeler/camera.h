/***************
 * PROGRAM:       Modeler
 * NAME:          camera.h
 * DESCRIPTION:   camera class definition
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    21.02.96 ah    initial release
 *    18.12.96 ah    added GetSize()
 ***************/

#ifndef CAMERA_H
#define CAMERA_H

#ifndef VECMATH_H
#include "vecmath.h"
#endif

#ifndef OBJECT_H
#include "object.h"
#endif

#define HFOV 25
#define VFOV 25

#define CAMERA_DEFAULT "Camera"

class CAMERA : public OBJECT
{
	public:
		float    hfov,vfov;  // field of view
		float    focaldist;  // focal distance
		float    aperture;   // camera aperture
		VECTOR   lookp;      // lookpoint

		CAMERA();
		virtual void SetDrawColor();
		virtual void Draw(DISPLAY*,MATRIX_STACK*);
		virtual void SetObject(DISPLAY*, VECTOR*, VECTOR*, VECTOR*, VECTOR*, VECTOR*);
		virtual BOOL WriteObject(struct IFFHandle *);
		virtual int GetType() { return OBJECT_CAMERA; }
		virtual rsiResult ToRSI(rsiCONTEXT*, MATRIX_STACK*, void**);
		virtual OBJECT *DupObj();
		virtual int GetSize();
#ifdef __AMIGA__
		virtual struct NewMenu *GetPopUpList();
#else
		virtual int GetResourceID();
#endif
		virtual BOOL IsVisible(DISPLAY*);
};

OBJECT *ParseCamera(struct IFFHandle *, OBJECT *, int);

#endif /* CAMERA_H */
