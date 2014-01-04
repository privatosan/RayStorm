/***************
 * PROGRAM:       Modeler
 * NAME:          camera.cpp
 * DESCRIPTION:   Functions for camera object class
 * AUTHORS:       Andreas Heumann & Mike Hesser
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    21.02.96 ah    initial release
 *    04.10.96 mh    bugfix: camera tracking
 *    02.11.96 ah    focal distance can now be set to track distance
 *    18.12.96 ah    added GetSize()
 ***************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef CAMERA_H
#include "camera.h"
#endif

#ifndef OGL_UTIL_H
#include "ogl_util.h"
#endif

#ifndef VIEW_H
#include "view.h"
#endif

#ifndef IFF_UTIL_H
#include "iff_util.h"
#endif

#ifndef RSCN_H
#include "rscn.h"
#endif

#ifndef GLOBALS_H
#include "globals.h"
#endif

#ifndef PROJECT_H
#include "project.h"
#endif

#ifndef __AMIGA__
#include "resource.h"
#else
#ifndef DEBUG_LIB
	#ifndef RSI_LIB_H
	#include "rsi_lib.h"
	#endif
#endif

struct NewMenu popuplist[] =
{
	{ NM_TITLE, NULL              , 0 ,0,0,(APTR)POP_NONE},
	{ NM_ITEM , "Move"            , 0 ,0,0,(APTR)POP_OBJECT_MOVE},
	{ NM_ITEM , "Rotate"          , 0 ,0,0,(APTR)POP_OBJECT_ROTATE},
	{ NM_ITEM , "Scale"           , 0 ,0,0,(APTR)POP_OBJECT_SCALE},
	{ NM_ITEM , "Settings..."     , 0 ,0,0,(APTR)POP_SETTINGS},
	{ NM_ITEM , "Active"          , 0 ,0,0,(APTR)POP_CAMERA_ACTIVE},
	{ NM_END,NULL,0,0,0,(APTR)0 },
};
#endif

/*************
 * DESCRIPTION:   Constructor of camera (sets the default values)
 * INPUT:         none
 * OUTPUT:        none
 *************/
CAMERA::CAMERA()
{
	SetName(CAMERA_DEFAULT);

	// camera-position
	SetVector(&pos, 0.f, 0.f, 0.f);

	SetVector(&orient_x, 1.f, 0.f, 0.f);
	SetVector(&orient_y, 0.f, 1.f, 0.f);
	SetVector(&orient_z, 0.f, 0.f, 1.f);

	flags |= OBJECT_CAMERA_VFOV;

	// Field of view
	hfov = tan(HFOV * .5f * PI_180);
	vfov = tan(VFOV * .5f * PI_180);

	// depth of field
	focaldist = 1.f;
	aperture = 0.f;

	SetVector(&bboxmin, -.5f, -.5f, -1.f);
	SetVector(&bboxmax, .5f, 1.3f, 1.5f);
}

/*************
 * DESCRIPTION:   gets the popuplist for the selected object
 * INPUT:         -
 * OUTPUT:        popup list
 *************/
#ifdef __AMIGA__
struct NewMenu *CAMERA::GetPopUpList()
{
	return popuplist;
}
#else
CAMERA::GetResourceID()
{
	return IDR_POPUP_CAMERA;
}
#endif

/*************
 * DESCRIPTION:   Set the color with wich the object is drawn
 * INPUT:         -
 * OUTPUT:        -
 *************/
void CAMERA::SetDrawColor()
{
	gfx.SetPen(COLOR_CAMERA);
}

/*************
 * DESCRIPTION:   Tests if an object is visible
 * INPUT:         display
 * OUTPUT:        TRUE if visible
 *************/
BOOL CAMERA::IsVisible(DISPLAY *display)
{
	return(display->filter_flags & FILTER_CAMERA);
}

/*************
 * DESCRIPTION:   Draw the camera
 * INPUT:         disp     display class
 *                stack    matrix stack
 * OUTPUT:        none
 *************/
void CAMERA::Draw(DISPLAY *disp, MATRIX_STACK *stack)
{
	VECTOR u,v;
	float dx,dy,dz,dx1,dy1,dz1;

	SetVector(&u, -size.z*.5f, -size.z*.5f, -size.z);
	SetVector(&v,  size.z*.5f,  size.z*.5f,  size.z);
	disp->DrawBox(stack,&u,&v);

	dx = size.z*.25f;
	dy = size.z*.25f;
	dz = size.z;
	dx1 = size.z*.40f;
	dy1 = size.z*.40f;
	dz1 = size.z*1.5f;

	switch(disp->display)
	{
		case DISPLAY_BBOX:
		case DISPLAY_WIRE:
			SetVector(&u, -dx, -dy,  dz);
			SetVector(&v,  dx, -dy,  dz);
			disp->TransformDraw(&u,&v,stack);

			v.x = -dx;
			v.y = dy;
			disp->TransformDraw(&u,&v,stack);

			SetVector(&v, -dx1, -dy1, dz1);
			disp->TransformDraw(&u,&v,stack);

			SetVector(&u,  dx, dy, dz);
			SetVector(&v, -dx, dy, dz);
			disp->TransformDraw(&u,&v,stack);

			v.x = dx;
			v.y = -dy;
			disp->TransformDraw(&u,&v,stack);

			SetVector(&v, dx1, dy1, dz1);
			disp->TransformDraw(&u,&v,stack);

			SetVector(&u, -dx1,  dy1, dz1);
			SetVector(&v, -dx1, -dy1, dz1);
			disp->TransformDraw(&u,&v,stack);

			v.x = dx1;
			v.y = dy1;
			disp->TransformDraw(&u,&v,stack);

			SetVector(&v, -dx, dy, dz);
			disp->TransformDraw(&u,&v,stack);

			SetVector(&u, dx1, -dy1, dz1);
			SetVector(&v, dx1,  dy1, dz1);
			disp->TransformDraw(&u,&v,stack);

			v.x = -dx1;
			v.y = -dy1;
			disp->TransformDraw(&u,&v,stack);

			v.x = dx;
			v.y = -dy;
			v.z = dz;
			disp->TransformDraw(&u,&v,stack);

			dx = size.z*1.3f;
			dy = size.z*.5f;
			dy1 = size.z*.9f;
			dz = size.z*.4f;
			dz1 = size.z*.8f;
			SetVector(&u, 0.f, dy, -dz);
			SetVector(&v, 0.f, dx,  dz);
			disp->TransformDraw(&u,&v,stack);

			v.y = dy1;
			v.z = -dz1;
			disp->TransformDraw(&u,&v,stack);

			u.y = dx;
			u.z = -dz;
			v.y = dy;
			v.z = dz;
			disp->TransformDraw(&u,&v,stack);

			v.y = dy1;
			v.z = -dz1;
			disp->TransformDraw(&u,&v,stack);

			u.y = dy1;
			u.z = dz1;
			v.y = dy;
			v.z = dz;
			disp->TransformDraw(&u,&v,stack);

			v.y = dx;
			disp->TransformDraw(&u,&v,stack);
			break;
#ifdef __OPENGL__
		case DISPLAY_SOLID:
			OpenGLDrawCamera(disp, dx,dy,dz, dx1,dy1,dz1, &size);
			break;
#endif // __OPENGL__
	}
}

/*************
 * DESCRIPTION:   sets the new object specs
 * INPUT:         disp     pointer to display structure
 *                pos      translate factor
 *                ox,oy,oz rotate factor
 *                size     scale factor
 * OUTPUT:        none
 *************/
void CAMERA::SetObject(DISPLAY *disp, VECTOR *pos, VECTOR *ox, VECTOR *oy, VECTOR *oz, VECTOR *size)
{
	MATRIX m;

	if(disp)
	{
		if(disp->view->viewmode == VIEW_CAMERA)
		{
			VecAdd(pos,&this->pos,&disp->view->pos);

			if(!track)
			{
				InvOrient(ox, oy, oz, &disp->view->axis_x, &disp->view->axis_y, &disp->view->axis_z);
				m.SetOMatrix(&orient_x,&orient_y,&orient_z);
				m.MultVectMat(&disp->view->axis_x);
				m.MultVectMat(&disp->view->axis_y);
				m.MultVectMat(&disp->view->axis_z);
			}
			else
			{
				UpdateTracking(&disp->view->pos);
				InvOrient(&orient_x, &orient_y, &orient_z, &disp->view->axis_x, &disp->view->axis_y, &disp->view->axis_z);
			}
		}
	}
	SetVector(&bboxmin, -this->size.z*.5f, -this->size.z*.5f, -this->size.z);
	SetVector(&bboxmax, this->size.z*.5f, this->size.z*1.3f, this->size.z*1.5f);
}

/*************
 * DESCRIPTION:   write camera to scene file
 * INPUT:         iff      iff handler
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL CAMERA::WriteObject(struct IFFHandle *iff)
{
	float data[2];

	if(PushChunk(iff, ID_CAMR, ID_FORM, IFFSIZE_UNKNOWN))
		return FALSE;

	if(!WriteCommon(iff))
		return FALSE;

	if(!WriteLongChunk(iff, ID_FDST, &focaldist, 1))
		return FALSE;

	if(!WriteLongChunk(iff, ID_APER, &aperture, 1))
		return FALSE;

	data[0] = hfov;
	data[1] = vfov;
	if(!WriteLongChunk(iff, ID_FDOV, data, 2))
		return FALSE;

	if(PopChunk(iff))
		return FALSE;

	return TRUE;
}

/*************
 * DESCRIPTION:   read a camera from scene file
 * INPUT:         iff      iff handler
 *                where    where to insert after
 *                dir      insert direction
 * OUTPUT:        created object
 *************/
OBJECT *ParseCamera(struct IFFHandle *iff, OBJECT *where, int dir)
{
	CAMERA *camera;
	struct ContextNode *cn;
	long error = 0;
	ULONG flags;

	camera = new CAMERA;
	if(!camera)
		return NULL;

	if(!camera->ReadAxis(iff))
	{
		delete camera;
		return NULL;
	}

	while(!error)
	{
		error = ParseIFF(iff, IFFPARSE_RAWSTEP);
		if(error < 0 && error != IFFERR_EOC)
		{
			delete camera;
			return NULL;
		}

		// Get a pointer to the context node describing the current context
		cn = CurrentChunk(iff);
		if(!cn)
			continue;

		if((cn->cn_ID == ID_FORM) && (cn->cn_Type == ID_CAMR))
			break;

		if(error == IFFERR_EOC)
		{
			error = 0;
			continue;
		}

		switch (cn->cn_ID)
		{
			case ID_NAME:
				if(!camera->ReadName(iff))
				{
					delete camera;
					return NULL;
				}
				break;
			case ID_TRCK:
				if(!camera->ReadTrack(iff))
				{
					delete camera;
					return NULL;
				}
				break;
			case ID_FLGS:
				if(rscn_version < 200)
				{
					if(!ReadULONG(iff, &flags, 1))
					{
						delete camera;
						return NULL;
					}
					if(flags & 1)
						camera->flags |= OBJECT_CAMERA_VFOV;
					else
						camera->flags &= ~OBJECT_CAMERA_VFOV;
					if(flags & 4)
						camera->flags |= OBJECT_CAMERA_FOCUSTRACK;
					else
						camera->flags &= ~OBJECT_CAMERA_FOCUSTRACK;
				}
				else
				{
					if(!camera->ReadFlags(iff))
					{
						delete camera;
						return NULL;
					}
				}
				break;
			case ID_FDST:
				if(!ReadFloat(iff,&camera->focaldist,1))
				{
					delete camera;
					return NULL;
				}
				break;
			case ID_APER:
				if(!ReadFloat(iff,&camera->aperture,1))
					return NULL;
				break;
			case ID_FDOV:
				if(!ReadFloat(iff,&camera->hfov,1))
				{
					delete camera;
					return NULL;
				}
				if(!ReadFloat(iff,&camera->vfov,1))
				{
					delete camera;
					return NULL;
				}
				break;
		}
	}

	SetVector(&camera->bboxmin, -camera->size.z*.5, -camera->size.z*.5, -camera->size.z);
	SetVector(&camera->bboxmax, camera->size.z*.5, camera->size.z*1.3, camera->size.z*1.5);

	camera->Insert(where,dir);

	return (OBJECT*)camera;
}

/*************
 * DESCRIPTION:   transfer camera data to RayStorm Interface
 * INPUT:         stack    matrix stack
 *                object   pointer to created rsi object
 * OUTPUT:        rsiERR_NONE if ok else error number
 *************/
rsiResult CAMERA::ToRSI(rsiCONTEXT *rc, MATRIX_STACK *stack, void **object)
{
	VECTOR up, look, orient_x, orient_y, orient_z, pos;
	MATRIX m, m1;
	int rsiflags;
	rsiResult err;

	stack->GenerateAxis(&orient_x, &orient_y, &orient_z, &pos);

	m.SetOMatrix(&orient_x, &orient_y, &orient_z);
	if(track)
	{
		track->GetObjectMatrix(&m1);
		m1.GenerateAxis(&orient_x, &orient_x, &orient_x, &look);
	}
	else
	{
		SetVector(&look, 0.f, 0.f, 1000.f);
		m.MultVectMat(&look);
		VecAdd(&look, &pos, &look);
	}

	SetVector(&up, 0.f, 1.f, 0.f);
	m.MultVectMat(&up);

	err = PPC_STUB(rsiSetCamera)(CTXT,
				rsiTCameraPos,    &pos,
				rsiTCameraViewUp, &up,
				rsiTCameraLook,   &look,
				rsiTDone);
	if(err)
		return err;

	if(flags & OBJECT_CAMERA_VFOV)
		vfov = hfov*global.yres/global.xres;

	if(flags & OBJECT_CAMERA_FOCUSTRACK)
	{
		VecSub(&look, &pos, &look);
		focaldist = VecNormalize(&look);
	}

	rsiflags = 0;

	if (flags & OBJECT_CAMERA_FASTDOF)
		rsiflags |= rsiFCameraFastDOF;

	if (flags & OBJECT_CAMERA_HIDDENAREA)
		rsiflags |= rsiFCameraHiddenArea;

	if (flags & OBJECT_CAMERA_ACCELERATE)
		rsiflags |= rsiFCameraAccelerate;

	return PPC_STUB(rsiSetCamera)(CTXT,
		rsiTCameraPos,       &pos,
		rsiTCameraHFov,      atan(hfov) * 2 * INV_PI_180,
		rsiTCameraVFov,      atan(vfov) * 2 * INV_PI_180,
		rsiTCameraFocalDist, focaldist,
		rsiTCameraAperture,  aperture,
		rsiTCameraFlags,     rsiflags,
		rsiTDone);
}

/*************
 * DESCRIPTION:   duplicate object
 * INPUT:         -
 * OUTPUT:        new object or NULL if failed
 *************/
OBJECT *CAMERA::DupObj()
{
	CAMERA *dup;

	dup = new CAMERA;
	if(!dup)
		return NULL;
	*dup = *this;
	return (OBJECT*)dup;
}

/*************
 * DESCRIPTION:   Get the size of a camera
 * INPUT:         -
 * OUTPUT:        size
 *************/
int CAMERA::GetSize()
{
	return sizeof(*this) + strlen(GetName()) + 1;
}
