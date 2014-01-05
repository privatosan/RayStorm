/***************
 * PROGRAM:       Modeler
 * NAME:          view.h
 * DESCRIPTION:   view class definition
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    10.12.94 ah    initial release
 ***************/

#ifndef VIEW_H
#define VIEW_H

#ifdef __AMIGA__
#include <exec/types.h>
#endif

#ifndef VECMATH_H
#include "vecmath.h"
#endif

#ifdef __OPENGL__
#ifndef OPENGL_H
#include "opengl.h"
#endif
#endif

class CAMERA;

// view mode
enum
{
	VIEW_FRONT, VIEW_RIGHT, VIEW_TOP, VIEW_PERSP, VIEW_CAMERA
};

enum
{
	CLIP_OUTSIDE, CLIP_INSIDE, CLIP_PARTIAL
};

typedef struct
{
	VECTOR min;
	VECTOR max;
	int left, top;       // dimension of rectangle
	int right, bottom;
	BOOL set;            // if TRUE, it is initialized else uninitialized
} RECTANGLE;

// view class
class VIEW
{
public:
	MATRIX MC_DC;
	ULONG viewmode;            // view mode: PERSP,FRONT,TOP,RIGHT,CAMERA
	SHORT left, top, right, bottom;
	SHORT width, height;       // window parameters

	// perspective view
	VECTOR pos;                // observer position
	VECTOR axis_x;             // camera orientation (camera looks to z direction,
	VECTOR axis_y;             // y direction is view up vector)
	VECTOR axis_z;
	VECTOR size;               // camera size (z is view plane distance,
										//              y is vertical field of view,
										//              x is horizontal field of view)
	VECTOR vpos;               // virtual camera look position
	VECTOR valign;             // virtual camera alignment to x-axis
	float dist;                // virtual camera distance
	RECTANGLE oldaccrect;      // bounding rectangle of previous object position

	// orthogonal view
	VECTOR viewpoint;          // viewpoint
	float zoom;                // zoomfactor

#ifdef __OPENGL__
	OPENGL opengl;
#endif

	VIEW();
	void ToDefaults();
	void SetCamera(CAMERA*);
	void Camera2Viewer(CAMERA*);
	void SetVirtualCamera();
	void SetViewRect(int, int, int, int, CAMERA*);
	void SetViewing(CAMERA*);
	void MCtoDC(VECTOR *);
	void MCtoDC_xy(VECTOR *);
	void MCtoDC2_xy(MATRIX*, VECTOR*, SHORT*, SHORT*);
	void MCtoDC2_xy_ortho(MATRIX*, VECTOR*, SHORT*, SHORT*);
	BOOL Clip3DLine(VECTOR*,VECTOR*);
	BOOL InView(VECTOR*);
	BOOL BBoxInView(VECTOR *, VECTOR *);
	MATRIX *GetViewMatrix();
};

#endif
