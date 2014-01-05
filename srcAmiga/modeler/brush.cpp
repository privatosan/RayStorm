/***************
 * PROGRAM:       Modeler
 * NAME:          brush.cpp
 * DESCRIPTION:   functions for brush class
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    29.06.96 ah    initial release
 *    18.12.96 ah    added GetSize()
 *    03.06.97 ah    added BRUSH_OBJECT
 ***************/

#include <string.h>

#ifndef TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef BRUSH_H
#include "brush.h"
#endif

#ifndef RSCN_H
#include "rscn.h"
#endif

#ifndef UTILITY_H
#include "utility.h"
#endif

#ifndef PREFS_H
#include "prefs.h"
#endif

#ifndef PROJECT_H
#include "project.h"
#endif

#ifdef __AMIGA__
static struct NewMenu popupmenu[] =
{
	{ NM_TITLE, NULL              , 0 ,0,0,(APTR)POP_NONE},
	{ NM_ITEM , "Move"            , 0 ,0,0,(APTR)POP_OBJECT_MOVE},
	{ NM_ITEM , "Rotate"          , 0 ,0,0,(APTR)POP_OBJECT_ROTATE},
	{ NM_ITEM , "Scale"           , 0 ,0,0,(APTR)POP_OBJECT_SCALE},
	{ NM_ITEM , "Settings..."     , 0 ,0,0,(APTR)POP_SETTINGS},
	{ NM_END,NULL,0,0,0,(APTR)0 },
};
#endif

/*************
 * DESCRIPTION:   constructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
BRUSH::BRUSH()
{
	file = NULL;
	type = BRUSH_MAP_COLOR;
	wrap = BRUSH_WRAP_FLAT;
	flags = BRUSH_SOFT;
}

/*************
 * DESCRIPTION:   destructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
BRUSH::~BRUSH()
{
	if(file)
		delete file;
}

/*************
 * DESCRIPTION:   set brush file name
 * INPUT:         newfile     new name
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL BRUSH::SetName(char *newfile)
{
	if(file)
		delete file;

	file = new char[strlen(newfile)+1];
	if(!file)
		return FALSE;

	strcpy(file, newfile);
	return TRUE;
}

/*************
 * DESCRIPTION:   write brush to scene file
 * INPUT:         iff      iff handle
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL BRUSH::Write(struct IFFHandle *iff)
{
	if(PushChunk(iff, ID_BRSH, ID_FORM, IFFSIZE_UNKNOWN))
		return FALSE;

	if(!WriteChunk(iff, ID_NAME, file, strlen(file)+1))
		return FALSE;

	if(!WriteLongChunk(iff, ID_FLGS, &flags, 1))
		return FALSE;

	if(!WriteLongChunk(iff, ID_TYPE, &type, 1))
		return FALSE;

	if(!WriteLongChunk(iff, ID_WRAP, &wrap, 1))
		return FALSE;

	if(PopChunk(iff))
		return FALSE;

	return TRUE;
}

/*************
 * DESCRIPTION:   parse a brush from a RSCN file
 * INPUT:         iff      iff handle
 *                surf     surface to add brush to
 * OUTPUT:        created brush
 *************/
BRUSH *ParseBrush(struct IFFHandle *iff, SURFACE *surf)
{
	BRUSH *brush;
	struct ContextNode *cn;
	long error = 0;
	char buffer[256];

	brush = new BRUSH;
	if(!brush)
		return NULL;

	while(!error)
	{
		error = ParseIFF(iff, IFFPARSE_RAWSTEP);
		if(error < 0 && error != IFFERR_EOC)
		{
			delete brush;
			return NULL;
		}

		// Get a pointer to the context node describing the current context
		cn = CurrentChunk(iff);
		if(!cn)
			continue;

		if(cn->cn_ID == ID_FORM)
			break;

		if(error == IFFERR_EOC)
		{
			error = 0;
			continue;
		}
		switch (cn->cn_ID)
		{
			case ID_NAME:
				if(!ReadString(iff,buffer,256))
				{
					delete brush;
					return NULL;
				}
				if(!brush->SetName(buffer))
				{
					delete brush;
					return NULL;
				}
				break;
			case ID_WRAP:
				if(!ReadULONG(iff,&brush->wrap,1))
				{
					delete brush;
					return NULL;
				}
				break;
			case ID_TYPE:
				if(!ReadULONG(iff,&brush->type,1))
				{
					delete brush;
					return NULL;
				}
				break;
			case ID_FLGS:
				if(!ReadULONG(iff,&brush->flags,1))
				{
					delete brush;
					return NULL;
				}
				break;
		}
	}

	surf->AddBrush(brush);

	return brush;
}

/*************
 * DESCRIPTION:   duplicate brush
 * INPUT:         -
 * OUTPUT:        new brush or NULL if failed
 *************/
BRUSH *BRUSH::DupObj()
{
	BRUSH *dup;

	dup = new BRUSH;
	if(!dup)
		return NULL;
	*dup = *this;
	dup->file = NULL;
	if(!dup->SetName(file))
	{
		delete dup;
		return NULL;
	}
	return dup;
}

/*************
 * DESCRIPTION:   Get the size of a brush
 * INPUT:         -
 * OUTPUT:        size
 *************/
int BRUSH::GetSize()
{
	return sizeof(BRUSH) + strlen(file) + 1;
}

/*************
 * DESCRIPTION:   constructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
BRUSH_OBJECT::BRUSH_OBJECT()
{
	brush = NULL;
}

/*************
 * DESCRIPTION:   Tests if an object is visible
 * INPUT:         display
 * OUTPUT:        TRUE if visible
 *************/
BOOL BRUSH_OBJECT::IsVisible(DISPLAY *display)
{
	return(display->filter_flags & FILTER_BRUSH);
}

/*************
 * DESCRIPTION:   Set the color with wich the object is drawn
 * INPUT:         -
 * OUTPUT:        -
 *************/
void BRUSH_OBJECT::SetDrawColor(DISPLAY *display)
{
#ifdef __OPENGL__
	if(display->display == DISPLAY_SOLID)
		SetGLColor(COLOR_BRUSH);
	else
#endif // __OPENGL__
	{
		if(selected)
			gfx.SetPen(COLOR_BRUSH);
		else
			gfx.SetPen(COLOR_UNSELECTED);
	}
}

/*************
 * DESCRIPTION:   gets the popuplist for the selected object
 * INPUT:         -
 * OUTPUT:        popup list
 *************/
#ifdef __AMIGA__
struct NewMenu *BRUSH_OBJECT::GetPopUpList()
{
	return popupmenu;
}
#endif

/*************
 * DESCRIPTION:   generate bounding box
 * INPUT:         none
 * OUTPUT:        none
 *************/
void BRUSH_OBJECT::CalcBBox()
{
	bboxmax = size;
	bboxmin.x = -size.x;
	bboxmin.y = -size.y;
	bboxmin.z = -size.z;
}

/*************
 * DESCRIPTION:   Draw a brush
 * INPUT:         disp     display class
 *                stack    matrix stack
 * OUTPUT:        none
 *************/
void BRUSH_OBJECT::Draw(DISPLAY *disp,MATRIX_STACK *stack)
{
	VECTOR size, trans;
	MATRIX m;

	switch(disp->display)
	{
		case DISPLAY_BBOX:
		case DISPLAY_WIRE:
			switch(brush->wrap)
			{
				case BRUSH_WRAP_FLAT:
					// draw a bounding box around pos
					disp->DrawBox(stack,&bboxmin,&bboxmax);
					break;
				case BRUSH_WRAP_X:
					SetVector(&size,
						(bboxmax.y - bboxmin.y) * .5f,
						bboxmax.x - bboxmin.x,
						(bboxmax.z - bboxmin.z) * .5f);
					m.SetRotZMatrix(90.f);
					stack->Push(&m);
					SetVector(&trans,
						0.f,
						- size.y * .5f,
						0.f);
					m.SetTransMatrix(&trans);
					stack->Push(&m);
					disp->DrawCylinder(stack, &size, flags);
					stack->Pop(&m);
					stack->Pop(&m);
					break;
				case BRUSH_WRAP_Y:
					SetVector(&size,
						(bboxmax.x - bboxmin.x) * .5f,
						bboxmax.y - bboxmin.y,
						(bboxmax.z - bboxmin.z) * .5f);
					SetVector(&trans,
						0.f,
						- size.y * .5f,
						0.f);
					m.SetTransMatrix(&trans);
					stack->Push(&m);
					disp->DrawCylinder(stack, &size, flags);
					stack->Pop(&m);
					break;
				case BRUSH_WRAP_XY:
					disp->DrawSphere(stack, (bboxmax.x-bboxmin.x)*.5f);
					break;
			}
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
void BRUSH_OBJECT::SetObject(DISPLAY *disp, VECTOR *pos, VECTOR *ox, VECTOR *oy, VECTOR *oz, VECTOR *size)
{
	CalcBBox();
}

/*************
 * DESCRIPTION:   write brush to scene file
 * INPUT:         iff      iff handler
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL BRUSH_OBJECT::WriteObject(struct IFFHandle *iff)
{
	VECTOR v[3];

	if(PushChunk(iff, ID_BRSH, ID_FORM, IFFSIZE_UNKNOWN))
		return FALSE;

	if(!WriteLongChunk(iff, ID_POSI, &pos, 3))
		return FALSE;

	v[0] = orient_x;
	v[1] = orient_y;
	v[2] = orient_z;
	if(!WriteLongChunk(iff, ID_ALGN, v, 9))
		return FALSE;

	if(!WriteLongChunk(iff, ID_SIZE, &size, 3))
		return FALSE;

	if(!WriteTrack(iff))
		return FALSE;

	if(!WriteFlags(iff))
		return FALSE;

	if(PopChunk(iff))
		return FALSE;

	return TRUE;
}

/*************
 * DESCRIPTION:   parse a brush object from a RSCN file
 * INPUT:         iff      iff handle
 *                obj      object to add brush to
 * OUTPUT:        created brush object
 *************/
BRUSH_OBJECT *ParseBrushObject(struct IFFHandle *iff, OBJECT *obj)
{
	BRUSH_OBJECT *brush;
	struct ContextNode *cn;
	long error = 0;

	brush = new BRUSH_OBJECT;
	if(!brush)
		return NULL;

	if(!brush->ReadAxis(iff))
	{
		delete brush;
		return NULL;
	}

	if(rscn_version >= 200)
	{
		while(!error)
		{
			error = ParseIFF(iff, IFFPARSE_RAWSTEP);
			if(error < 0 && error != IFFERR_EOC)
			{
				delete brush;
				return NULL;
			}

			// Get a pointer to the context node describing the current context
			cn = CurrentChunk(iff);
			if(!cn)
				continue;

			if((cn->cn_ID == ID_FORM) && (cn->cn_Type == ID_BRSH))
				break;

			if(error == IFFERR_EOC)
			{
				error = 0;
				continue;
			}
			switch (cn->cn_ID)
			{
				case ID_TRCK:
					if(!brush->ReadTrack(iff))
					{
						delete brush;
						return NULL;
					}
					break;
				case ID_FLGS:
					if(!brush->ReadFlags(iff))
					{
						delete brush;
						return NULL;
					}
					break;
			}
		}
	}

	obj->AddBrushTop(brush);

	return brush;
}

/*************
 * DESCRIPTION:   duplicate object
 * INPUT:         -
 * OUTPUT:        new object or NULL if failed
 *************/
OBJECT *BRUSH_OBJECT::DupObj()
{
	BRUSH_OBJECT *dup;

	dup = new BRUSH_OBJECT;
	if(!dup)
		return NULL;
	*dup = *this;
	return (OBJECT*)dup;
}

/*************
 * DESCRIPTION:   transfer brush data to RayStorm Interface
 * INPUT:         surf     current surface
 *                obj      current object
 *                stack    matrix stack
 * OUTPUT:        rsiERR_NONE if ok else error number
 *************/
rsiResult BRUSH_OBJECT::ToRSI(rsiCONTEXT *rc, void *surf, OBJECT *obj, MATRIX_STACK *stack)
{
	rsiResult err;
	void *brush;
	ULONG rsiflags=0;
	VECTOR ox,oy,oz;
	VECTOR orient_x, orient_y, orient_z, pos;
	MATRIX m;
	BOOL retry;
	char buffer[256];

	m.SetOTMatrix(&this->orient_x,&this->orient_y,&this->orient_z,&this->pos);
	stack->Push(&m);

	stack->GenerateAxis(&orient_x, &orient_y, &orient_z, &pos);

	stack->Pop();

	retry = FALSE;
	do
	{
		err = PPC_STUB(rsiAddBrush)(CTXT, &brush, GetName(), surf);
		if(err)
		{
			// can't load brush -> ask user for other filename
			PPC_STUB(rsiGetErrorMsg)(CTXT, buffer, err);
			retry = utility.AskRequest("Can't load brush '%s':\n%s\n\nDo you want to change the filename?", GetName(), buffer);
			if(!retry)
				return err;
			else
			{
				strcpy(buffer, GetName());
				retry = utility.FileRequ(buffer, FILEREQU_BRUSH, FILEREQU_INITIALFILEDIR);
				if(retry)
					SetName(buffer);
				else
					return err;
			}
		}
	}
	while(retry && err);

	switch(this->brush->type)
	{
		case BRUSH_MAP_COLOR:
			rsiflags |= rsiFBrushColor;
			break;
		case BRUSH_MAP_REFLECTIVITY:
			rsiflags |= rsiFBrushReflectivity;
			break;
		case BRUSH_MAP_FILTER:
			rsiflags |= rsiFBrushFilter;
			break;
		case BRUSH_MAP_ALTITUDE:
			rsiflags |= rsiFBrushAltitude;
			break;
		case BRUSH_MAP_SPECULAR:
			rsiflags |= rsiFBrushSpecular;
			break;
	}

	switch(this->brush->wrap)
	{
		case BRUSH_WRAP_X:
			rsiflags |= rsiFBrushWrapX;
			break;
		case BRUSH_WRAP_Y:
			rsiflags |= rsiFBrushWrapY;
			break;
		case BRUSH_WRAP_XY:
			rsiflags |= rsiFBrushWrapXY;
			break;
	}
	if(this->brush->flags & BRUSH_SOFT)
		rsiflags |= rsiFBrushSoft;
	if(this->brush->flags & BRUSH_REPEAT)
		rsiflags |= rsiFBrushRepeat;
	if(this->brush->flags & BRUSH_MIRROR)
		rsiflags |= rsiFBrushMirror;

	InvOrient(&orient_x, &orient_y, &orient_z, &ox, &oy, &oz);

	return PPC_STUB(rsiSetBrush)(CTXT, brush,
		rsiTBrushFlags,      rsiflags,
		rsiTBrushPos,        &pos,
		rsiTBrushOrientX,    &ox,
		rsiTBrushOrientY,    &oy,
		rsiTBrushOrientZ,    &oz,
		rsiTBrushSize,       &size,
		rsiTDone);
}

/*************
 * DESCRIPTION:   Get the size of a brush object
 * INPUT:         -
 * OUTPUT:        size
 *************/
int BRUSH_OBJECT::GetSize()
{
	return sizeof(BRUSH_OBJECT);
}
