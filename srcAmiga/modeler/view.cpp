/***************
 * PROJECT:       RayStorm Scenario
 * MODULE:        view.cpp
 * DESCRIPTION:   This module provides the view structure used to describe the
 *  position of the viepoint and the structure of the view
 *  volume.  This view volume is used for clipping everything
 *  outside of it out of the raster list.  A view structure can
 *  be allocated and then configured to be used as the current
 *  view. The view is controlled by specifying a point you are
 *  looking at and then spherical coordinates to the point you are
 *  looking from.
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    10.12.96 ah    initial release
 *    10.10.96 ah    added Camera2Viewer()
 *    08.01.97 ah    added undo to Camera2Viewer()
 ***************/

#ifndef ERRORS_H
#include "errors.h"
#endif

#ifndef VECMATH_H
#include "vecmath.h"
#endif

#ifndef DISPLAY_H
#include "display.h"
#endif

#ifndef VIEW_H
#include "view.h"
#endif

#ifndef CAMERA_H
#include "camera.h"
#endif

#ifndef GLOBALS_H
#include "globals.h"
#endif

/*************
 * DESCRIPTION:   Constructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
VIEW::VIEW()
{
	top = 0;
	left = 0;

	ToDefaults();
}

/*************
 * DESCRIPTION:   Sets viewer position of virtual camera
 * INPUT:         none
 * OUTPUT:        none
 *************/
void VIEW::SetVirtualCamera()
{
	pos.x = sin(valign.y)*cos(valign.x)*dist;
	pos.y = cos(valign.y)*dist;
	pos.z = sin(valign.y)*sin(valign.x)*dist;
	axis_y.x = sin(valign.y - PI_2)*cos(valign.x);
	axis_y.y = cos(valign.y - PI_2);
	axis_y.z = sin(valign.y - PI_2)*sin(valign.x);
	axis_z.x = -pos.x;
	axis_z.y = -pos.y;
	axis_z.z = -pos.z;
	VecNormalize(&axis_z);
	VecCross(&axis_y,&axis_z,&axis_x);
	VecAdd(&pos,&vpos,&pos);
	SetViewing(NULL);
}

/*************
 * DESCRIPTION:   set all values to defaults
 * INPUT:         none
 * OUTPUT:        none
 *************/
void VIEW::ToDefaults()
{
	viewmode = VIEW_PERSP;

	// Position of camera (polar)
	SetVector(&vpos, 0.f,0.f,0.f);
	SetVector(&valign, 1.25f*PI, .25f*PI, 0.f);
	dist = 30;

	SetVector(&viewpoint, 0.f,0.f,0.f);
	zoom = 50;

	SetVirtualCamera();
}

/*************
 * DESCRIPTION:   Sets viewer position of real camera
 * INPUT:         camera
 * OUTPUT:        none
 *************/
void VIEW::SetCamera(CAMERA *camera)
{
	pos = camera->pos;

	// invert camera orientation to get view directions
	InvOrient(&camera->orient_x, &camera->orient_y, &camera->orient_z,
		&axis_x, &axis_y, &axis_z);

	SetViewing(camera);
}

/*************
 * DESCRIPTION:   Sets camera to virtual viewer position
 * INPUT:         camera
 * OUTPUT:        none
 *************/
void VIEW::Camera2Viewer(CAMERA *camera)
{
	VECTOR ox,oy,oz;
	UNDO_TRANSFORM *undo;

	undo = new UNDO_TRANSFORM();
	if (undo)
	{
		undo->AddTransformed(camera);
		undo->Add();
	}

	camera->pos.x = sin(valign.y)*cos(valign.x)*dist;
	camera->pos.y = cos(valign.y)*dist;
	camera->pos.z = sin(valign.y)*sin(valign.x)*dist;

	oy.x = sin(valign.y - PI_2)*cos(valign.x);
	oy.y = cos(valign.y - PI_2);
	oy.z = sin(valign.y - PI_2)*sin(valign.x);
	oz.x = -camera->pos.x;
	oz.y = -camera->pos.y;
	oz.z = -camera->pos.z;
	VecNormalize(&oz);
	VecCross(&oy,&oz,&ox);
	VecAdd(&camera->pos,&vpos,&camera->pos);

	// invert camera orientation to get view directions
	InvOrient(&ox, &oy, &oz, &camera->orient_x, &camera->orient_y, &camera->orient_z);

	camera->hfov = .5f;
	camera->vfov = camera->hfov*global.yres/global.xres;
}

/*************
 * DESCRIPTION:   sets view bounds
 * INPUT:         l,t,r,b  view bounds
 *                camera   camera
 * OUTPUT:        -
 *************/
void VIEW::SetViewRect(int l, int t, int r, int b, CAMERA *camera)
{
	left = l;
	top = t;
	right = r;
	bottom = b;
	width = right - left + 1;
	height = bottom - top + 1;
	SetViewing(camera);
#ifdef __OPENGL__
	opengl.Viewport(l, t, width, height);
#endif
}

/*************
 * DESCRIPTION:   sets viewmatrix
 * INPUT:         camera
 * OUTPUT:        -
 *************/
void VIEW::SetViewing(CAMERA *camera)
{
	MATRIX WC_VRC, VRC_NPC, NPC_WC, WC_NPC, NPC_DC;
	VECTOR v;

	size.x = 1.0f;
	size.y = (float)height/width;

	if ((viewmode == VIEW_PERSP || viewmode == VIEW_CAMERA) && camera)
	{
		// set aspect
		if (viewmode == VIEW_CAMERA)
		{
			if (camera->flags & OBJECT_CAMERA_VFOV)
				camera->vfov = camera->hfov*global.yres/global.xres;
			if (float(width)/float(height) > camera->hfov/camera->vfov)
			{
				size.y = camera->vfov * 2.f;
				size.x = float(width)/float(height) * size.y;
			}
			else
			{
				size.x = camera->hfov * 2.f;
				size.y = float(height)/float(width) * size.x;
			}
		}
		else
		{
			size.y = float(height)/float(width) * size.x;
		}
		size.z = 1.f;

		// calculate w-axis (origin)
		WC_VRC.m[0] = 1.f;
		WC_VRC.m[1] = pos.x - axis_z.x;
		WC_VRC.m[2] = pos.y - axis_z.y;
		WC_VRC.m[3] = pos.z - axis_z.z;

		// calculate z-axis (view plane normal)
		WC_VRC.m[3*4  ] = 0.f;
		WC_VRC.m[3*4+1] = axis_z.x;
		WC_VRC.m[3*4+2] = axis_z.y;
		WC_VRC.m[3*4+3] = axis_z.z;

		// insert y-axis (view up vector)
		WC_VRC.m[2*4  ] = 0.f;
		WC_VRC.m[2*4+1] = axis_y.x;
		WC_VRC.m[2*4+2] = axis_y.y;
		WC_VRC.m[2*4+3] = axis_y.z;

		// insert x-axis (direction to right)
		WC_VRC.m[  4  ] = 0.f;
		WC_VRC.m[  4+1] = axis_x.x;
		WC_VRC.m[  4+2] = axis_x.y;
		WC_VRC.m[  4+3] = axis_x.z;

		// calculate w-axis (origin)
		VRC_NPC.m[0] = 1.f;
		VRC_NPC.m[1] = size.x;
		VRC_NPC.m[2] = size.y;
		VRC_NPC.m[3] = -1.f;

		// calculate x-axis (origin)
		VRC_NPC.m[  4  ] = 0.f;
		VRC_NPC.m[  4+1] = -2.f*size.x;
		VRC_NPC.m[  4+2] = 0.f;
		VRC_NPC.m[  4+3] = 0.f;

		// calculate y-axis (origin)
		VRC_NPC.m[2*4  ] = 0.f;
		VRC_NPC.m[2*4+1] = 0.f;
		VRC_NPC.m[2*4+2] = -2.f*size.y;
		VRC_NPC.m[2*4+3] = 0.f;

		// calculate z-axis (origin)
		VRC_NPC.m[3*4  ] = 1.f;
		VRC_NPC.m[3*4+1] = 0.f;
		VRC_NPC.m[3*4+2] = 0.f;
		VRC_NPC.m[3*4+3] = size.z;

		NPC_WC.MultMat(&VRC_NPC, &WC_VRC);
		NPC_WC.InvMat(&WC_NPC);

		// Screentransformation
		// set matrix to identity matrix
		NPC_DC.IdentityMatrix();
		NPC_DC.m[  4+1] = width;      // x scale
		NPC_DC.m[2*4+2] = -height;    // y scale
		NPC_DC.m[3*4+3] = 1.f;        // z scale
		NPC_DC.m[    1] = left;
		NPC_DC.m[    2] = bottom;

		MC_DC.MultMat(&WC_NPC, &NPC_DC);
	}
	else
	{
		SetVector(&v, -viewpoint.x, -viewpoint.y, -viewpoint.z);
		WC_VRC.SetTMatrix(&v);

		switch (viewmode)
		{
			case VIEW_FRONT:
				SetVector(&v, zoom, -zoom, zoom);
				VRC_NPC.SetSMatrix(&v);

				NPC_WC.MultMat(&WC_VRC, &VRC_NPC);

				SetVector(&v, (width>>1) + left, (height>>1) + top, 0.f);
				VRC_NPC.SetTMatrix(&v);
				MC_DC.MultMat(&NPC_WC, &VRC_NPC);
				break;
			case VIEW_RIGHT:
				SetVector(&v, zoom, -zoom, zoom);
				VRC_NPC.SetSMatrix(&v);

				NPC_WC.MultMat(&WC_VRC, &VRC_NPC);

				SetVector(&v, 0.f, (height>>1) + top, (width>>1) + left);
				VRC_NPC.SetTMatrix(&v);
				MC_DC.MultMat(&NPC_WC, &VRC_NPC);
				MC_DC.m[1] = MC_DC.m[3];
				MC_DC.m[5] = MC_DC.m[7];
				MC_DC.m[9] = MC_DC.m[11];
				MC_DC.m[13] = MC_DC.m[15];
				break;
			case VIEW_TOP:
				SetVector(&v, zoom, zoom, -zoom);
				VRC_NPC.SetSMatrix(&v);

				NPC_WC.MultMat(&WC_VRC, &VRC_NPC);

				SetVector(&v, (width>>1) + left, 0.f,  (height>>1) + top);
				VRC_NPC.SetTMatrix(&v);
				MC_DC.MultMat(&NPC_WC, &VRC_NPC);
				MC_DC.m[2] = MC_DC.m[3];
				MC_DC.m[6] = MC_DC.m[7];
				MC_DC.m[10] = MC_DC.m[11];
				MC_DC.m[14] = MC_DC.m[15];
				break;
		}
	}
}

/*************
 * DESCRIPTION:   Transforms a point (modell coordinates) to screen coordinates
 *                (Perspective)
 * INPUT:         matrix   view matrix
 *                v        vector to transform
 *                xx,yy    result
 * OUTPUT:        none
 *************/
void VIEW::MCtoDC2_xy(MATRIX *m, VECTOR *v, SHORT *xx, SHORT *yy)
{
	float x,y,z, w;

	x = v->x;
	y = v->y;
	z = v->z;
	// affin->homogen, Object->Screen, homogen->affin
	w = m->m[0] + x*m->m[4] + y*m->m[8] + z*m->m[12];
	if (w != 0.)
		w = 1.f/w;

	*xx = (SHORT)((m->m[1] + x*m->m[5] + y*m->m[9] + z*m->m[13])*w);
	*yy = (SHORT)((m->m[2] + x*m->m[6] + y*m->m[10] + z*m->m[14])*w);
}

/*************
 * DESCRIPTION:   Transforms a point (modell coordinates) to screen coordinates
 *                (Perspective). Assumes that m is homogen.
 * INPUT:         matrix   view matrix
 *                v        vector to transform
 *                xx,yy    result
 * OUTPUT:        none
 *************/
void VIEW::MCtoDC2_xy_ortho(MATRIX *m, VECTOR *v, SHORT *xx, SHORT *yy)
{
	*xx = (SHORT)((m->m[1] + v->x*m->m[5] + v->y*m->m[9] + v->z*m->m[13]));
	*yy = (SHORT)((m->m[2] + v->x*m->m[6] + v->y*m->m[10] + v->z*m->m[14]));
}

/*************
 * DESCRIPTION:   Transforms a point (modell coordinates) to screen coordinates
 *    (device coordinates). Ignores z.
 * INPUT:         v        vector to transform
 * OUTPUT:        none
 *************/
void VIEW::MCtoDC_xy(VECTOR *v)
{
	float w,x,y,z;

	x = v->x;
	y = v->y;
	z = v->z;
	// affin->homogen, Object->Screen, homogen->affin
	w = MC_DC.m[0] + x*MC_DC.m[4] + y*MC_DC.m[8] + z*MC_DC.m[12];
	if (w != 0.f)
		w = 1.f/w;

	v->x = (MC_DC.m[1] + x*MC_DC.m[5] + y*MC_DC.m[9] + z*MC_DC.m[13])*w;
	v->y = (MC_DC.m[2] + x*MC_DC.m[6] + y*MC_DC.m[10] + z*MC_DC.m[14])*w;
}

/*************
 * DESCRIPTION:   Transforms a point (modell coordinates) to screen coordinates
 *    (device coordinates).
 * INPUT:         v        vector to transform
 * OUTPUT:        none
 *************/
void VIEW::MCtoDC(VECTOR *v)
{
	float w,x,y,z;

	x = v->x;
	y = v->y;
	z = v->z;
	// affin->homogen, Object->Screen, homogen->affin
	w = MC_DC.m[0] + x*MC_DC.m[4] + y*MC_DC.m[8] + z*MC_DC.m[12];
	if (w != 0.f)
		w = 1.f/w;

	v->x = (MC_DC.m[1] + x*MC_DC.m[5] + y*MC_DC.m[9] + z*MC_DC.m[13])*w;
	v->y = (MC_DC.m[2] + x*MC_DC.m[6] + y*MC_DC.m[10] + z*MC_DC.m[14])*w;
	v->z = (MC_DC.m[3] + x*MC_DC.m[7] + y*MC_DC.m[11] + z*MC_DC.m[15])*w;
}

/*************
 * DESCRIPTION:   Clips line behind the camera
 * INPUT:         v1       start point
 *                v2       end point
 * OUTPUT:        CLIP_OUTSIDE if completely outside
 *                CLIP_INSIDE if completely inside
 *                CLIP_PARTIAL if partial in and outside
 *************/
BOOL VIEW::Clip3DLine(VECTOR *v1,VECTOR *v2)
{
	VECTOR dir,d1,d2;
	float c1,c2,d,t;
	VECTOR pos;

	pos.x = this->pos.x + axis_z.x;
	pos.y = this->pos.y + axis_z.y;
	pos.z = this->pos.z + axis_z.z;

	VecSub(v1, &pos, &d1);
	VecSub(v2, &pos, &d2);
	c1 = dotp(&axis_z, &d1);
	c2 = dotp(&axis_z, &d2);
	if (c1 > 0.f && c2 > 0.f)   // whole line is before view plane
		return CLIP_INSIDE;
	if (c1 < 0.f && c2 < 0.f)   // whole line is behind view plane
		return CLIP_OUTSIDE;

	VecSub(v2, v1, &dir);

	d = dotp(&axis_z, &dir);
	if (fabs(d) < EPSILON)
		return CLIP_OUTSIDE;    // line is on view plane

	t = (dotp(&axis_z, &pos) - dotp(&axis_z, v1)) / d;

	if (c1 > 0.f)
		VecAddScaled(v1, t, &dir, v2);
	else
		VecAddScaled(v1, t, &dir, v1);

	return CLIP_PARTIAL;
}

/*************
 * DESCRIPTION:   Tests if a point is before view plane
 * INPUT:         p        point
 * OUTPUT:        TRUE if before view plane
 *************/
BOOL VIEW::InView(VECTOR *p)
{
	VECTOR d;
	float c;

	if((viewmode != VIEW_PERSP) && (viewmode != VIEW_CAMERA))
		return TRUE;

	VecSub(p, &pos, &d);
	c = dotp(&axis_z, &d);
	if (c > 0.f)    // point is before view plane
		return TRUE;

	return FALSE;
}

/*************
 * DESCRIPTION:   Tests if a bounding box is before view plane
 * INPUT:         max, min bounding box
 * OUTPUT:        TRUE if before view plane
 *************/
BOOL VIEW::BBoxInView(VECTOR *max, VECTOR *min)
{
	VECTOR v;

	v = *max;
	if (InView(&v))
		return TRUE;

	v.x = min->x;
	if (InView(&v))
		return TRUE;

	v.y = min->y;
	if (InView(&v))
		return TRUE;

	v.x = max->x;
	if (InView(&v))
		return TRUE;

	v.z = min->z;
	if (InView(&v))
		return TRUE;

	v.y = max->y;
	if (InView(&v))
		return TRUE;

	v.x = min->x;
	if (InView(&v))
		return TRUE;

	v.y = min->y;
	if (InView(&v))
		return TRUE;

	return FALSE;
}

/*************
 * DESCRIPTION:   Return the current view matrix
 * INPUT:         -
 * OUTPUT:        view matrix
 *************/
MATRIX *VIEW::GetViewMatrix()
{
	return &MC_DC;
}
