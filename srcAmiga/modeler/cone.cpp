/***************
 * PROGRAM:       Modeler
 * NAME:          conde.cpp
 * DESCRIPTION:   Functions for cone object class
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

#ifndef CONE_H
#include "cone.h"
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
 * DESCRIPTION:   Constructor of cone (sets the default values)
 * INPUT:         none
 * OUTPUT:        none
 *************/
CONE::CONE()
{
	SetName("Cone");
	SetVector(&bboxmin, -size.x, 0.f, -size.z);
}

/*************
 * DESCRIPTION:   Tests if an object is visible
 * INPUT:         display
 * OUTPUT:        TRUE if visible
 *************/
BOOL CONE::IsVisible(DISPLAY *display)
{
	return(display->filter_flags & FILTER_CONE);
}

/*************
 * DESCRIPTION:   Draw a cone
 * INPUT:         disp     display class
 *                stack    matrix stack
 * OUTPUT:        none
 *************/
void CONE::Draw(DISPLAY *disp,MATRIX_STACK *stack)
{
	VECTOR bottom,bottom1,centerbottom, centertop;
	int i;
	MATRIX m;
	SHORT xbottom,ybottom, xbottom1,ybottom1, xcenterbottom,ycenterbottom;
	SHORT xcentertop,ycentertop;

	switch(disp->display)
	{
		case DISPLAY_BBOX:
			// draw a bounding box around pos
			disp->DrawBox(stack,&bboxmin,&bboxmax);
			break;
		case DISPLAY_WIRE:
			// draw a wireframe cone
			if((disp->view->viewmode == VIEW_PERSP) || (disp->view->viewmode == VIEW_CAMERA))
			{
				SetVector(&bottom, 0.f, 0.f, size.z);
				SetVector(&centerbottom, 0.f, 0.f, 0.f);
				SetVector(&centertop, 0.f, size.y, 0.f);
				for(i=CONE_DIVS; i<=360; i+=CONE_DIVS)
				{
					disp->TransformDraw(&bottom,&centertop,stack);

					SetVector(&bottom1, sin(i*PI_180)*size.x, 0.f, cos(i*PI_180)*size.z);
					disp->TransformDraw(&bottom,&bottom1,stack);
					if(!(flags & OBJECT_OPENBOTTOM))
						disp->TransformDraw(&bottom, &centerbottom, stack);
					bottom = bottom1;
				}
			}
			else
			{
				m.MultMat(stack->GetMatrix(), disp->view->GetViewMatrix());

				SetVector(&bottom, 0.f, 0.f, size.z);
				disp->view->MCtoDC2_xy_ortho(&m, &bottom, &xbottom, &ybottom);
				SetVector(&centerbottom, 0.f, 0.f, 0.f);
				disp->view->MCtoDC2_xy_ortho(&m, &centerbottom, &xcenterbottom, &ycenterbottom);

				SetVector(&centertop, 0.f, size.y, 0.f);
				disp->view->MCtoDC2_xy_ortho(&m, &centertop, &xcentertop, &ycentertop);

				for(i=CONE_DIVS; i<=360; i+=CONE_DIVS)
				{
					gfx.ClipLine(xbottom,ybottom, xcentertop,ycentertop);

					SetVector(&bottom1, sin(i*PI_180)*size.x, 0.f, cos(i*PI_180)*size.z);
					disp->view->MCtoDC2_xy_ortho(&m, &bottom1, &xbottom1, &ybottom1);
					gfx.ClipLine(xbottom,ybottom, xbottom1,ybottom1);
					if(!(flags & OBJECT_OPENBOTTOM))
						gfx.ClipLine(xbottom,ybottom, xcenterbottom,ycenterbottom);
					xbottom = xbottom1;
					ybottom = ybottom1;
				}
			}
			break;
#ifdef __OPENGL__
		case DISPLAY_SOLID:
			OpenGLDrawCone(&size, flags);
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
void CONE::SetObject(DISPLAY *disp, VECTOR *pos, VECTOR *ox, VECTOR *oy, VECTOR *oz, VECTOR *size)
{
	CalcBBox();
}

/*************
 * DESCRIPTION:   write cone to scene file
 * INPUT:         iff      iff handler
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL CONE::WriteObject(struct IFFHandle *iff)
{
	if(PushChunk(iff, ID_CONE, ID_FORM, IFFSIZE_UNKNOWN))
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
 * DESCRIPTION:   parse a cone from a RSCN file
 * INPUT:         iff      iff handler
 *                where    where to insert after
 *                dir      insert direction
 * OUTPUT:        created object
 *************/
OBJECT *ParseCone(struct IFFHandle *iff, OBJECT *where, int dir)
{
	CONE *cone;
	struct ContextNode *cn;
	long error = 0;

	cone = new CONE;
	if(!cone)
		return NULL;

	if(!cone->ReadAxis(iff))
	{
		delete cone;
		return NULL;
	}
	cone->surf = GetSurfaceByName(SURF_DEFAULTNAME);

	while(!error)
	{
		error = ParseIFF(iff, IFFPARSE_RAWSTEP);
		if(error < 0 && error != IFFERR_EOC)
		{
			delete cone;
			return NULL;
		}

		// Get a pointer to the context node describing the current context
		cn = CurrentChunk(iff);
		if(!cn)
			continue;

		if((cn->cn_ID == ID_FORM) && (cn->cn_Type == ID_CONE))
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
						if(!ReadSurface(iff, &cone->surf, (OBJECT*)cone))
						{
							delete cone;
							return NULL;
						}
						break;
				}
				break;
			case ID_NAME:
				if(!cone->ReadName(iff))
				{
					delete cone;
					return NULL;
				}
				break;
			case ID_TRCK:
				if(!cone->ReadTrack(iff))
				{
					delete cone;
					return NULL;
				}
				break;
			case ID_FLGS:
				if(!cone->ReadFlags(iff))
				{
					delete cone;
					return NULL;
				}
				break;
		}
	}

	SetVector(&cone->bboxmin, -cone->size.x, 0., -cone->size.z);
	cone->bboxmax = cone->size;
	cone->Insert(where, dir);

	return (OBJECT*)cone;
}

/*************
 * DESCRIPTION:   transfer cone data to RayStorm Interface
 * INPUT:         stack    matrix stack
 *                object   pointer to created rsi object
 * OUTPUT:        rsiERR_NONE if ok else error number
 *************/
rsiResult CONE::ToRSI(rsiCONTEXT *rc, MATRIX_STACK *stack, void **object)
{
	rsiResult err;
	void *surface;
	VECTOR orient_x, orient_y, orient_z, pos;
	int flags;

	stack->GenerateAxis(&orient_x, &orient_y, &orient_z, &pos);

	err = surf->ToRSI(rc, &surface,stack,(OBJECT*)this);
	if(err)
		return err;

	err = PPC_STUB(rsiCreateCone)(CTXT, object, surface);
	if(err)
		return err;

	flags = 0;
	if(this->flags & OBJECT_OPENBOTTOM)
		flags |= rsiFConeOpenBottom;
	if(this->flags & OBJECT_INVERTED)
		flags |= rsiFConeInverted;

	return PPC_STUB(rsiSetCone)(CTXT, *object,
		rsiTConePos,      &pos,
		rsiTConeSize,     &size,
		rsiTConeOrientX,  &orient_x,
		rsiTConeOrientY,  &orient_y,
		rsiTConeOrientZ,  &orient_z,
		rsiTConeFlags,    flags,
		rsiTDone);
}

/*************
 * DESCRIPTION:   duplicate object
 * INPUT:         -
 * OUTPUT:        new object or NULL if failed
 *************/
OBJECT *CONE::DupObj()
{
	CONE *dup;

	dup = new CONE;
	if(!dup)
		return NULL;
	*dup = *this;
	return (OBJECT*)dup;
}

/*************
 * DESCRIPTION:   Get the size of a cone
 * INPUT:         -
 * OUTPUT:        size
 *************/
int CONE::GetSize()
{
	return sizeof(*this) + strlen(GetName()) + 1;
}

/*************
 * DESCRIPTION:   calculate bounding box for object
 * INPUT:         none
 * OUTPUT:        none
 *************/
void CONE::CalcBBox()
{
	SetVector(&bboxmin, -size.x, 0., -size.z);
	bboxmax = size;
}
