/***************
 * PROGRAM:       Modeler
 * NAME:          sphere.cpp
 * DESCRIPTION:   Functions for sphere object class
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    10.12.95 ah    initial release
 *    18.12.96 ah    added GetSize()
 ***************/

#include <string.h>

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef OBJECT_H
#include "object.h"
#endif

#ifndef SPHERE_H
#include "sphere.h"
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

/*************
 * DESCRIPTION:   Constructor of sphere (sets the default values)
 * INPUT:         none
 * OUTPUT:        none
 *************/
SPHERE::SPHERE()
{
	SetName("Sphere");
}

/*************
 * DESCRIPTION:   Tests if an object is visible
 * INPUT:         display
 * OUTPUT:        TRUE if visible
 *************/
BOOL SPHERE::IsVisible(DISPLAY *display)
{
	return(display->filter_flags & FILTER_SPHERE);
}

/*************
 * DESCRIPTION:   Draw a sphere
 * INPUT:         disp     display class
 *                stack    matrix stack
 * OUTPUT:        none
 *************/
void SPHERE::Draw(DISPLAY *disp,MATRIX_STACK *stack)
{
	switch(disp->display)
	{
		case DISPLAY_BBOX:
			// draw a bounding box around pos
			disp->DrawBox(stack,&bboxmin,&bboxmax);
			break;
		case DISPLAY_WIRE:
		case DISPLAY_SOLID:
			disp->DrawSphere(stack, size.x);
			break;
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
void SPHERE::SetObject(DISPLAY *disp, VECTOR *pos, VECTOR *ox, VECTOR *oy, VECTOR *oz, VECTOR *size)
{
	this->size.y = this->size.x;
	this->size.z = this->size.x;
	SetVector(&bboxmin, -this->size.x, -this->size.y, -this->size.z);
	bboxmax = this->size;
}

/*************
 * DESCRIPTION:   write sphere to scene file
 * INPUT:         iff      iff handler
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL SPHERE::WriteObject(struct IFFHandle *iff)
{
	if(PushChunk(iff, ID_SPHR, ID_FORM, IFFSIZE_UNKNOWN))
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
 * DESCRIPTION:   parse a sphere from a RSCN file
 * INPUT:         iff      iff handler
 *                where    where to insert after
 *                dir      insert direction
 * OUTPUT:        created object
 *************/
OBJECT *ParseSphere(struct IFFHandle *iff, OBJECT *where, int dir)
{
	SPHERE *sphere;
	struct ContextNode *cn;
	long error = 0;

	sphere = new SPHERE;
	if(!sphere)
		return NULL;

	if(!sphere->ReadAxis(iff))
	{
		delete sphere;
		return NULL;
	}
	sphere->surf = GetSurfaceByName(SURF_DEFAULTNAME);

	while(!error)
	{
		error = ParseIFF(iff, IFFPARSE_RAWSTEP);
		if(error < 0 && error != IFFERR_EOC)
		{
			delete sphere;
			return NULL;
		}

		// Get a pointer to the context node describing the current context
		cn = CurrentChunk(iff);
		if(!cn)
			continue;

		if((cn->cn_ID == ID_FORM) && (cn->cn_Type == ID_SPHR))
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
						if(!ReadSurface(iff, &sphere->surf, (OBJECT*)sphere))
						{
							delete sphere;
							return NULL;
						}
						break;
				}
				break;
			case ID_NAME:
				if(!sphere->ReadName(iff))
				{
					delete sphere;
					return NULL;
				}
				break;
			case ID_TRCK:
				if(!sphere->ReadTrack(iff))
				{
					delete sphere;
					return NULL;
				}
				break;
			case ID_FLGS:
				if(!sphere->ReadFlags(iff))
				{
					delete sphere;
					return NULL;
				}
				break;
		}
	}

	SetVector(&sphere->bboxmin, -sphere->size.x, -sphere->size.y, -sphere->size.z);
	sphere->bboxmax = sphere->size;
	sphere->Insert(where, dir);

	return (OBJECT*)sphere;
}

/*************
 * DESCRIPTION:   transfer sphere data to RayStorm Interface
 * INPUT:         stack    matrix stack
 *                object   pointer to created rsi object
 * OUTPUT:        rsiERR_NONE if ok else error number
 *************/
rsiResult SPHERE::ToRSI(rsiCONTEXT *rc, MATRIX_STACK *stack, void **object)
{
	rsiResult err;
	void *surface;
	VECTOR orient_x, orient_y, orient_z, pos;
	int flags;

	stack->GenerateAxis(&orient_x, &orient_y, &orient_z, &pos);

	err = surf->ToRSI(rc, &surface,stack,(OBJECT*)this);
	if(err)
		return err;

	err = PPC_STUB(rsiCreateSphere)(CTXT, object, surface);
	if(err)
		return err;

	flags = 0;
	if(this->flags & OBJECT_INVERTED)
		flags |= rsiFCSGInverted;

	return PPC_STUB(rsiSetSphere)(CTXT, *object,
		rsiTSpherePos,       &pos,
		rsiTSphereRadius,    size.x,
		rsiTSphereFlags,     flags,
		rsiTDone);
}

/*************
 * DESCRIPTION:   duplicate object
 * INPUT:         -
 * OUTPUT:        new object or NULL if failed
 *************/
OBJECT *SPHERE::DupObj()
{
	SPHERE *dup;

	dup = new SPHERE;
	if(!dup)
		return NULL;
	*dup = *this;
	return (OBJECT*)dup;
}

/*************
 * DESCRIPTION:   Get the size of a sphere
 * INPUT:         -
 * OUTPUT:        size
 *************/
int SPHERE::GetSize()
{
	return sizeof(*this) + strlen(GetName()) + 1;
}

/*************
 * DESCRIPTION:   calculate bounding box for object
 * INPUT:         none
 * OUTPUT:        none
 *************/
void SPHERE::CalcBBox()
{
	SetVector(&bboxmin, -size.x, -size.y, -size.z);
	bboxmax = size;
}
