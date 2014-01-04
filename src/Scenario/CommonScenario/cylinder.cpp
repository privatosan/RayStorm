/***************
 * PROGRAM:       Modeler
 * NAME:          cylinder.cpp
 * DESCRIPTION:   Functions for cylinder object class
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    25.03.97 ah    initial release
 ***************/

#include <string.h>

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef OBJECT_H
#include "object.h"
#endif

#ifndef CYLINDER_H
#include "cylinder.h"
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
 * DESCRIPTION:   Constructor of cylinder (sets the default values)
 * INPUT:         none
 * OUTPUT:        none
 *************/
CYLINDER::CYLINDER()
{
	SetName("Cylinder");
	SetVector(&bboxmin, -size.x, 0.f, -size.z);
}

/*************
 * DESCRIPTION:   calculate bounding box for object
 * INPUT:         none
 * OUTPUT:        none
 *************/
void CYLINDER::CalcBBox()
{
	SetVector(&bboxmin, -size.x, 0.f, -size.z);
	bboxmax = size;
}

/*************
 * DESCRIPTION:   Tests if an object is visible
 * INPUT:         display
 * OUTPUT:        TRUE if visible
 *************/
BOOL CYLINDER::IsVisible(DISPLAY *display)
{
	return(display->filter_flags & FILTER_CYLINDER);
}

/*************
 * DESCRIPTION:   Draw a cylinder
 * INPUT:         disp     display class
 *                stack    matrix stack
 * OUTPUT:        none
 *************/
void CYLINDER::Draw(DISPLAY *disp,MATRIX_STACK *stack)
{
	switch(disp->display)
	{
		case DISPLAY_BBOX:
			// draw a bounding box around pos
			disp->DrawBox(stack,&bboxmin,&bboxmax);
			break;
		case DISPLAY_WIRE:
		case DISPLAY_SOLID:
			disp->DrawCylinder(stack, &size, flags);
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
void CYLINDER::SetObject(DISPLAY *disp, VECTOR *pos, VECTOR *ox, VECTOR *oy, VECTOR *oz, VECTOR *size)
{
	CalcBBox();
}

/*************
 * DESCRIPTION:   write cylinder to scene file
 * INPUT:         iff      iff handler
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL CYLINDER::WriteObject(struct IFFHandle *iff)
{
	if(PushChunk(iff, ID_CYLR, ID_FORM, IFFSIZE_UNKNOWN))
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
 * DESCRIPTION:   parse a cylinder from a RSCN file
 * INPUT:         iff      iff handler
 *                where    where to insert after
 *                dir      insert direction
 * OUTPUT:        created object
 *************/
OBJECT *ParseCylinder(struct IFFHandle *iff, OBJECT *where, int dir)
{
	CYLINDER *cylinder;
	struct ContextNode *cn;
	long error = 0;

	cylinder = new CYLINDER;
	if(!cylinder)
		return NULL;

	if(!cylinder->ReadAxis(iff))
	{
		delete cylinder;
		return NULL;
	}
	cylinder->surf = GetSurfaceByName(SURF_DEFAULTNAME);

	while(!error)
	{
		error = ParseIFF(iff, IFFPARSE_RAWSTEP);
		if(error < 0 && error != IFFERR_EOC)
		{
			delete cylinder;
			return NULL;
		}

		// Get a pointer to the context node describing the current context
		cn = CurrentChunk(iff);
		if(!cn)
			continue;

		if((cn->cn_ID == ID_FORM) && (cn->cn_Type == ID_CYLR))
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
						if(!ReadSurface(iff, &cylinder->surf, (OBJECT*)cylinder))
						{
							delete cylinder;
							return NULL;
						}
						break;
				}
				break;
			case ID_NAME:
				if(!cylinder->ReadName(iff))
				{
					delete cylinder;
					return NULL;
				}
				break;
			case ID_TRCK:
				if(!cylinder->ReadTrack(iff))
				{
					delete cylinder;
					return NULL;
				}
				break;
			case ID_FLGS:
				if(!cylinder->ReadFlags(iff))
				{
					delete cylinder;
					return NULL;
				}
				break;
		}
	}

	SetVector(&cylinder->bboxmin, -cylinder->size.x, 0., -cylinder->size.z);
	cylinder->bboxmax = cylinder->size;
	cylinder->Insert(where, dir);

	return (OBJECT*)cylinder;
}

/*************
 * DESCRIPTION:   transfer cylinder data to RayStorm Interface
 * INPUT:         stack    matrix stack
 *                object   pointer to created rsi object
 * OUTPUT:        rsiERR_NONE if ok else error number
 *************/
rsiResult CYLINDER::ToRSI(rsiCONTEXT *rc, MATRIX_STACK *stack, void **object)
{
	rsiResult err;
	void *surface;
	VECTOR orient_x, orient_y, orient_z, pos;
	int flags;

	stack->GenerateAxis(&orient_x, &orient_y, &orient_z, &pos);

	err = surf->ToRSI(rc, &surface,stack,(OBJECT*)this);
	if(err)
		return err;

	err = PPC_STUB(rsiCreateCylinder)(CTXT, object, surface);
	if(err)
		return err;

	flags = 0;
	if(this->flags & OBJECT_OPENTOP)
		flags |= rsiFCylinderOpenTop;
	if(this->flags & OBJECT_OPENBOTTOM)
		flags |= rsiFCylinderOpenBottom;
	if(this->flags & OBJECT_INVERTED)
		flags |= rsiFCylinderInverted;

	return PPC_STUB(rsiSetCylinder)(CTXT, *object,
		rsiTCylinderPos,     &pos,
		rsiTCylinderSize,    &size,
		rsiTCylinderOrientX, &orient_x,
		rsiTCylinderOrientY, &orient_y,
		rsiTCylinderOrientZ, &orient_z,
		rsiTCylinderFlags,   flags,
		rsiTDone);
}

/*************
 * DESCRIPTION:   duplicate object
 * INPUT:         -
 * OUTPUT:        new object or NULL if failed
 *************/
OBJECT *CYLINDER::DupObj()
{
	CYLINDER *dup;

	dup = new CYLINDER;
	if(!dup)
		return NULL;
	*dup = *this;
	return (OBJECT*)dup;
}

/*************
 * DESCRIPTION:   Get the size of a cylinder
 * INPUT:         -
 * OUTPUT:        size
 *************/
int CYLINDER::GetSize()
{
	return sizeof(*this) + strlen(GetName()) + 1;
}
