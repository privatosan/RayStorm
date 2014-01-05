/***************
 * PROGRAM:       Modeler
 * NAME:          box.cpp
 * DESCRIPTION:   Functions for box object class
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    11.01.97 ah    initial release
 ***************/

#include <string.h>

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef OBJECT_H
#include "object.h"
#endif

#ifndef BOX_H
#include "box.h"
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

#ifndef SCI_H
#include "sci.h"
#endif

#ifndef BRUSH_H
#include "brush.h"
#endif

/*************
 * DESCRIPTION:   Constructor of box (sets the default values)
 * INPUT:         none
 * OUTPUT:        none
 *************/
BOX::BOX()
{
	SetName("Box");
}

/*************
 * DESCRIPTION:   Tests if an object is visible
 * INPUT:         display
 * OUTPUT:        TRUE if visible
 *************/
BOOL BOX::IsVisible(DISPLAY *display)
{
	return(display->filter_flags & FILTER_BOX);
}

/*************
 * DESCRIPTION:   Draw a box
 * INPUT:         disp     display class
 *                stack    matrix stack
 * OUTPUT:        none
 *************/
void BOX::Draw(DISPLAY *disp, MATRIX_STACK *stack)
{
	switch(disp->display)
	{
		case DISPLAY_BBOX:
		case DISPLAY_WIRE:
		case DISPLAY_SOLID:
			// draw a bounding box around pos
			disp->DrawBox(stack,&bboxmin,&bboxmax);
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
void BOX::SetObject(DISPLAY *disp, VECTOR *pos, VECTOR *ox, VECTOR *oy, VECTOR *oz, VECTOR *size)
{
}

/*************
 * DESCRIPTION:   write box to scene file
 * INPUT:         iff      iff handler
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL BOX::WriteObject(struct IFFHandle *iff)
{
	if(PushChunk(iff, ID_BOX, ID_FORM, IFFSIZE_UNKNOWN))
		return FALSE;

	if(!WriteCommon(iff))
		return FALSE;

	if(!WriteSurface(iff))
		return FALSE;

	if(!WriteLongChunk(iff, ID_LBND, &bboxmin, 3))
		return FALSE;

	if(!WriteLongChunk(iff, ID_HBND, &bboxmax, 3))
		return FALSE;

	if(PopChunk(iff))
		return FALSE;

	return TRUE;
}

/*************
 * DESCRIPTION:   parse a box from a RSCN file
 * INPUT:         iff      iff handler
 *                where    where to insert after
 *                dir      insert direction
 * OUTPUT:        created object
 *************/
OBJECT *ParseBox(struct IFFHandle *iff, OBJECT *where, int dir)
{
	BOX *box;
	struct ContextNode *cn;
	long error = 0;

	box = new BOX;
	if(!box)
		return NULL;

	if(!box->ReadAxis(iff))
	{
		delete box;
		return NULL;
	}
	box->surf = GetSurfaceByName(SURF_DEFAULTNAME);

	while(!error)
	{
		error = ParseIFF(iff, IFFPARSE_RAWSTEP);
		if(error < 0 && error != IFFERR_EOC)
		{
			delete box;
			return NULL;
		}

		// Get a pointer to the context node describing the current context
		cn = CurrentChunk(iff);
		if(!cn)
			continue;

		if((cn->cn_ID == ID_FORM) && (cn->cn_Type == ID_BOX))
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
						if(!ReadSurface(iff, &box->surf, (OBJECT*)box))
						{
							delete box;
							return NULL;
						}
						break;
				}
				break;
			case ID_NAME:
				if(!box->ReadName(iff))
				{
					delete box;
					return NULL;
				}
				break;
			case ID_TRCK:
				if(!box->ReadTrack(iff))
				{
					delete box;
					return NULL;
				}
				break;
			case ID_FLGS:
				if(!box->ReadFlags(iff))
				{
					delete box;
					return NULL;
				}
				break;
			case ID_LBND:
				if(!ReadFloat(iff,(float*)&box->bboxmin,3))
				{
					delete box;
					return NULL;
				}
				break;
			case ID_HBND:
				if(!ReadFloat(iff,(float*)&box->bboxmax,3))
				{
					delete box;
					return NULL;
				}
				break;
		}
	}

	box->Insert(where, dir);

	return (OBJECT*)box;
}

/*************
 * DESCRIPTION:   transfer box data to RayStorm Interface
 * INPUT:         stack    matrix stack
 *                object   pointer to created rsi object
 * OUTPUT:        rsiERR_NONE if ok else error number
 *************/
rsiResult BOX::ToRSI(rsiCONTEXT *rc, MATRIX_STACK *stack, void **object)
{
	rsiResult err;
	void *surface;
	VECTOR orient_x, orient_y, orient_z, pos;
	int flags;

	stack->GenerateAxis(&orient_x, &orient_y, &orient_z, &pos);

	err = surf->ToRSI(rc, &surface,stack,(OBJECT*)this);
	if(err)
		return err;

	err = PPC_STUB(rsiCreateBox)(CTXT, object, surface);
	if(err)
		return err;

	flags = 0;
	if(this->flags & OBJECT_INVERTED)
		flags |= rsiFCSGInverted;

	return PPC_STUB(rsiSetBox)(CTXT, *object,
		rsiTBoxPos,          &pos,
		rsiTBoxLowBounds,    &bboxmin,
		rsiTBoxHighBounds,   &bboxmax,
		rsiTBoxOrientX,      &orient_x,
		rsiTBoxOrientY,      &orient_y,
		rsiTBoxOrientZ,      &orient_z,
		rsiTBoxFlags,        flags,
		rsiTDone);
}

/*************
 * DESCRIPTION:   duplicate object
 * INPUT:         -
 * OUTPUT:        new object or NULL if failed
 *************/
OBJECT *BOX::DupObj()
{
	BOX *dup;

	dup = new BOX;
	if(!dup)
		return NULL;
	*dup = *this;
	return (OBJECT*)dup;
}

/*************
 * DESCRIPTION:   Get the size of a box
 * INPUT:         -
 * OUTPUT:        size
 *************/
int BOX::GetSize()
{
	return sizeof(*this) + strlen(GetName()) + 1;
}
