/***************
 * PROGRAM:       Modeler
 * NAME:          plane.cpp
 * DESCRIPTION:   Functions for plane object class
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    03.06.96 ah    initial release
 *    18.12.96 ah    added GetSize()
 *    07.01.97 ah    bounding box has not been updated in SetObject()
 *                   plane is now independent scaleable in x and z direction
 ***************/

#include <string.h>

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef OBJECT_H
#include "object.h"
#endif

#ifndef PLANE_H
#include "plane.h"
#endif

#ifndef VIEW_H
#include "view.h"
#endif

#ifndef RSCN_H
#include "rscn.h"
#endif

#ifndef PROJECT_H
#include "project.h"
#endif

#ifndef MAGIC_H
#include "magic.h"
#endif

#ifndef SCI_H
#include "sci.h"
#endif

#ifndef BRUSH_H
#include "brush.h"
#endif

#ifndef OPENGL_UTIL_H
#include "ogl_util.h"
#endif

/*************
 * DESCRIPTION:   Constructor of plane (sets the default values)
 * INPUT:         none
 * OUTPUT:        none
 *************/
PLANE::PLANE()
{
	SetName("Plane");
}

/*************
 * DESCRIPTION:   Tests if an object is visible
 * INPUT:         display
 * OUTPUT:        TRUE if visible
 *************/
BOOL PLANE::IsVisible(DISPLAY *display)
{
	return(display->filter_flags & FILTER_PLANE);
}

/*************
 * DESCRIPTION:   Draw a plane
 * INPUT:         disp     display class
 *                stack    matrix stack
 * OUTPUT:        none
 *************/
void PLANE::Draw(DISPLAY *disp,MATRIX_STACK *stack)
{
	VECTOR u,v;
	int i;
	float delta;

	switch(disp->display)
	{
		case DISPLAY_BBOX:
		case DISPLAY_WIRE:
			delta = size.x / PLANE_DIVS;
			SetVector(&u, -size.x, 0.f,  size.z);
			SetVector(&v, -size.x, 0.f, -size.z);
			for(i=-PLANE_DIVS; i<=PLANE_DIVS; i++)
			{
				disp->TransformDraw(&u,&v,stack);
				u.x += delta;
				v.x += delta;
			}
			delta = size.z / PLANE_DIVS;
			SetVector(&u,  size.x, 0.f, -size.z);
			SetVector(&v, -size.x, 0.f, -size.z);
			for(i=-PLANE_DIVS; i<=PLANE_DIVS; i++)
			{
				disp->TransformDraw(&u,&v,stack);
				u.z += delta;
				v.z += delta;
			}
			break;
#ifdef __OPENGL__
		case DISPLAY_SOLID:
			OpenGLDrawPlane(&size);
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
void PLANE::SetObject(DISPLAY *disp, VECTOR *pos, VECTOR *ox, VECTOR *oy, VECTOR *oz, VECTOR *size)
{
	CalcBBox();
}

/*************
 * DESCRIPTION:   write plane to scene file
 * INPUT:         iff      iff handler
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL PLANE::WriteObject(struct IFFHandle *iff)
{
	if(PushChunk(iff, ID_PLAN, ID_FORM, IFFSIZE_UNKNOWN))
		return FALSE;

	if(!WriteCommon(iff))
		return FALSE;

	if(!WriteSurface(iff))
		return FALSE;

	if(PopChunk(iff))
		return FALSE;

	return TRUE;
}

/*************
 * DESCRIPTION:   parse a plane from a RSCN file
 * INPUT:         iff      iff handler
 *                where    where to insert after
 *                dir      insert direction
 * OUTPUT:        created object
 *************/
OBJECT *ParsePlane(struct IFFHandle *iff, OBJECT *where, int dir)
{
	PLANE *plane;
	struct ContextNode *cn;
	long error = 0;

	plane = new PLANE;
	if(!plane)
		return NULL;

	if(!plane->ReadAxis(iff))
	{
		delete plane;
		return NULL;
	}
	plane->surf = GetSurfaceByName(SURF_DEFAULTNAME);

	while(!error)
	{
		error = ParseIFF(iff, IFFPARSE_RAWSTEP);
		if(error < 0 && error != IFFERR_EOC)
		{
			delete plane;
			return NULL;
		}

		// Get a pointer to the context node describing the current context
		cn = CurrentChunk(iff);
		if(!cn)
			continue;

		if((cn->cn_ID == ID_FORM) && (cn->cn_Type == ID_PLAN))
			break;

		if(error == IFFERR_EOC)
		{
			error = 0;
			continue;
		}
		switch (cn->cn_ID)
		{
			case ID_FORM:
				switch(cn->cn_Type)
				{
					case ID_SURF:
						if(!ReadSurface(iff, &plane->surf, (OBJECT*)plane))
						{
							delete plane;
							return NULL;
						}
						break;
				}
				break;
			case ID_NAME:
				if(!plane->ReadName(iff))
				{
					delete plane;
					return NULL;
				}
				break;
			case ID_TRCK:
				if(!plane->ReadTrack(iff))
				{
					delete plane;
					return NULL;
				}
				break;
			case ID_FLGS:
				if(!plane->ReadFlags(iff))
				{
					delete plane;
					return NULL;
				}
				break;
		}
	}

	plane->Insert(where, dir);

	return (OBJECT*)plane;
}

/*************
 * DESCRIPTION:   transfer plane data to RayStorm Interface
 * INPUT:         stack    matrix stack
 *                object   pointer to created rsi object
 * OUTPUT:        rsiERR_NONE if ok else error number
 *************/
rsiResult PLANE::ToRSI(rsiCONTEXT *rc, MATRIX_STACK *stack, void **object)
{
	rsiResult err;
	void *surface;
	VECTOR orient_x, orient_y, orient_z, pos;
	VECTOR up, v;

	stack->GenerateAxis(&orient_x, &orient_y, &orient_z, &pos);

	err = surf->ToRSI(rc, &surface,stack,(OBJECT*)this);
	if(err)
		return err;

	err = PPC_STUB(rsiCreatePlane)(CTXT, object, surface);
	if(err)
		return err;

	SetVector(&v, 0.f, 1.f, 0.f);
	up.x = dotp(&orient_x, &v);
	up.y = dotp(&orient_y, &v);
	up.z = dotp(&orient_z, &v);

	return PPC_STUB(rsiSetPlane)(CTXT, *object,
		rsiTPlanePos,     &pos,
		rsiTPlaneNorm,    &up,
		rsiTDone);
}

/*************
 * DESCRIPTION:   duplicate object
 * INPUT:         -
 * OUTPUT:        new object or NULL if failed
 *************/
OBJECT *PLANE::DupObj()
{
	PLANE *dup;

	dup = new PLANE;
	if(!dup)
		return NULL;
	*dup = *this;
	return (OBJECT*)dup;
}

/*************
 * DESCRIPTION:   Get the size of a plane
 * INPUT:         -
 * OUTPUT:        size
 *************/
int PLANE::GetSize()
{
	return sizeof(*this) + strlen(GetName()) + 1;
}

/*************
 * DESCRIPTION:   calculate bounding box for object
 * INPUT:         none
 * OUTPUT:        none
 *************/
void PLANE::CalcBBox()
{
	bboxmax = size;
	SetVector(&bboxmin, -this->size.x, -this->size.y, -this->size.z);
}
