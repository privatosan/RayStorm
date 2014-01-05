/***************
 * PROGRAM:       Modeler
 * NAME:          object.cpp
 * DESCRIPTION:   This modul contains all the functions for the rootclass
 *                OBJECT
 * AUTHORS:       Andreas Heumann
 * HISTORY:
 *    DATE     NAME  COMMENT
 *    10.12.95 ah    initial release
 *    01.12.96 ah    copyied and cuted child objects had wrong positions and orientations
 *    15.12.96 ah    added tracking to all objects
 *    16.12.96 ah    added undo to delete and cut; added DuplicateObjects()
 *    18.12.96 ah    added GetSizeOfTree()
 *    07.01.97 ah    added DeleteObject()
 *    30.01.97 ah    added DeChainObject()
 *    06.02.97 ah    added TestSelectedBox()
 *    22.02.97 ah    added TestSelectedPoints()
 *    27.02.97 ah    added SelectObject() and DeslectObject()
 *    28.02.97 ah    added UpdateTracking()
 *    03.06.97 ah    added AddBrush()
 *    19.06.97 ah    added WriteSurface()
 *    20.08.97 ah    GetObjects() and GetSelectedObjects() has now a parameter to specify
 *       if brushes or textures should be returned too
 ***************/

#include <stdio.h>
#include <string.h>

#ifdef __AMIGA__
	#include <pragma/dos_lib.h>
	#include <pragma/exec_lib.h>
#endif

#ifndef OBJECT_H
#include "object.h"
#endif

#ifndef MESH_H
#include "mesh.h"
#endif

#ifndef SPHERE_H
#include "sphere.h"
#endif

#ifndef BOX_H
#include "box.h"
#endif

#ifndef CYLINDER_H
#include "cylinder.h"
#endif

#ifndef CONE_H
#include "cone.h"
#endif

#ifndef PLANE_H
#include "plane.h"
#endif

#ifndef DISPLAY_H
#include "display.h"
#endif

#ifndef ERRORS_H
#include "errors.h"
#endif

#ifndef FLARES_H
#include "flares.h"
#endif

#ifndef STAR_H
#include "star.h"
#endif

#ifndef CAMERA_H
#include "camera.h"
#endif

#ifndef OBJHAND_H
#include "objhand.h"
#endif

#ifndef RSCN_H
#include "rscn.h"
#endif

#ifndef MAGIC_H
#include "magic.h"
#endif

#ifndef SCI_H
#include "sci.h"
#endif

#ifndef PROJECT_H
#include "project.h"
#endif

#ifndef BRUSH_H
#include "brush.h"
#endif

#ifndef TEXTURE_H
#include "texture.h"
#endif

#ifndef UTILITY_H
#include "utility.h"
#endif

#ifndef OPENGL_UTIL_H
#include "ogl_util.h"
#endif

#ifndef GLOBALS_H
#include "globalS.h"
#endif

#ifdef __AMIGA__

#include "rmod:objhand_lib.h"
// uncomment the lines below to debug an object loader module (and comment the line above)
/*extern "C"
{
	BOOL objInit();
	char *objRead(rsiCONTEXT*, char*, OBJLINK *, const VECTOR*, const VECTOR*, const VECTOR*, const VECTOR*, const VECTOR*, ACTOR *, SURFACE *, ULONG*);
	void objCleanup();
}*/

#ifndef DRAW_CLASS_H
#include "draw_class.h"
#endif

#else

#include "resource.h"
#include "camview.h"

#endif

#ifndef GFX_H
#include "gfx.h"
#endif

#ifndef __AMIGA__
typedef BOOL *(*INITPROC)();
typedef char *(*READWRITEPROC)(rsiCONTEXT*, char *, OBJLINK *, const VECTOR *, const VECTOR *, const VECTOR *, const VECTOR *, const VECTOR *, ACTOR *, SURFACE *, ULONG *);
typedef void *(*CLEANUPPROC)();
#endif

#ifdef __AMIGA__

struct Library *ObjHandBase;

#else
extern char szWorkingDirectory[256];
#include "fileutil.h"
#endif

extern GFX gfx;

static char *objerror = NULL;

#ifdef __AMIGA__
static struct NewMenu popupmenu[] =
{
	{ NM_TITLE, NULL              , 0 ,0,0,(APTR)POP_NONE},
	{ NM_ITEM , "Move"            , 0 ,0,0,(APTR)POP_OBJECT_MOVE},
	{ NM_ITEM , "Rotate"          , 0 ,0,0,(APTR)POP_OBJECT_ROTATE},
	{ NM_ITEM , "Scale"           , 0 ,0,0,(APTR)POP_OBJECT_SCALE},
	{ NM_ITEM , "Settings..."     , 0 ,0,0,(APTR)POP_SETTINGS},
	{ NM_ITEM , "Attributes..."   , 0 ,0,0,(APTR)POP_ATTRIBUTE},
	{ NM_END,NULL,0,0,0,(APTR)0 },
};

#ifdef __MIXEDBINARY__
BOOL ppc_objInit();
extern "C" char *ppc_objRead(rsiCONTEXT*, char*, OBJLINK*, VECTOR*, VECTOR*, VECTOR*, VECTOR*, VECTOR*, ACTOR*, SURFACE*, ULONG*);
void ppc_objCleanup();
#endif

#endif

// root for object tree
static OBJECT *root = NULL;
// root for paste buffer
static OBJECT *buffer = NULL;
// currently selected object for select next and prev
static OBJECT *cur_selected = NULL;
// first selected object
static OBJECT *firstselected = NULL;

/*************
 * DESCRIPTION:   Iterate through a hierarchical object list
 * INPUT:         stack    matrix stack
 * OUTPUT:        next object
 *************/
OBJECT *OBJECT::Iterate(MATRIX_STACK *stack)
{
	OBJECT *obj = this;

	if (obj->GoDown())
	{
		obj = (OBJECT*)obj->GoDown();
	}
	else
	{
		while (obj && !obj->GetNext())
		{
			stack->Pop();
			while(obj->GetPrev())
				obj = (OBJECT*)obj->GetPrev();
			obj = (OBJECT*)obj->GoUp();
		}
		if (obj)
			obj = (OBJECT*)obj->GetNext();
		stack->Pop();
	}
	return obj;
}

/*************
 * DESCRIPTION:   Iterate through a hierarchical object list
 * INPUT:         -
 * OUTPUT:        next object
 *************/
OBJECT *OBJECT::Iterate()
{
	OBJECT *obj = this;

	if (obj->GoDown())
		obj = (OBJECT*)obj->GoDown();
	else
	{
		while (obj && !obj->GetNext())
		{
			while(obj->GetPrev())
				obj = (OBJECT*)obj->GetPrev();
			obj = (OBJECT*)obj->GoUp();
		}
		if (obj)
			obj = (OBJECT*)obj->GetNext();
	}
	return obj;
}

/*************
 * DESCRIPTION:   Constructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
OBJECT::OBJECT()
{
	name = NULL;
	surf = NULL;
	brush = NULL;
	texture = NULL;
	SetVector(&pos, 0.f, 0.f, 0.f);
	SetVector(&orient_x, 1.f, 0.f, 0.f);
	SetVector(&orient_y, 0.f, 1.f, 0.f);
	SetVector(&orient_z, 0.f, 0.f, 1.f);
	SetVector(&size, 1.f, 1.f, 1.f);
	SetVector(&bboxmax, 1.f, 1.f, 1.f);
	SetVector(&bboxmin, -1.f, -1.f, -1.f);
	selected = FALSE;
	istracked = FALSE;
	track = NULL;
	flags = 0;//OBJECT_DIRTY;
	external = EXTERNAL_NONE;
	win = NULL;
	opencnt = 0;
}

/*************
 * DESCRIPTION:   Destructor
 * INPUT:         none
 * OUTPUT:        none
 *************/
OBJECT::~OBJECT()
{
	if(!opencnt && win)
	{
#ifdef __AMIGA__
		// close dialog
		SetAttrs(win, MUIA_Window_Open, FALSE, TAG_DONE);
		if(app)
		{
			DoMethod(app, OM_REMMEMBER, win);
			MUI_DisposeObject(win);
		}
#else
// close transform dialog
#endif
	}

	if(brush)
		RemoveBrushes();
	if(texture)
		RemoveTextures();
	if(name)
		delete name;
}

/*************
 * DESCRIPTION:   Inserts an object in the object list
 * INPUT:         where       where to insert after
 *                dir         direction
 * OUTPUT:        none
 *************/
void OBJECT::Insert(OBJECT *where,int dir)
{
	OBJECT *oldroot;

	ASSERT((dir == INSERT_HORIZ) || (dir == INSERT_VERT));

	if(where)
		this->LIST::Insert((LIST*)where,dir);
	else
	{
		if(root)
		{
			oldroot = root;
			InitWithoutDown();
			root = this;
			oldroot->LIST::Append((LIST**)&root);
		}
		else
			root = this;
	}
}

/*************
 * DESCRIPTION:   Appends an object to the object list
 * INPUT:         -
 * OUTPUT:        -
 *************/
void OBJECT::Append()
{
	this->LIST::Append((LIST**)&root);
}

/*************
 * DESCRIPTION:   Removes an object from the object list (without childs)
 * INPUT:         -
 * OUTPUT:        -
 *************/
void OBJECT::Remove()
{
	if(root)
		this->LIST::Remove((LIST**)&root);
}

/*************
 * DESCRIPTION:   Inserts a object to the object buffer
 * INPUT:         where       where to insert after
 *                dir         direction
 * OUTPUT:        none
 *************/
void OBJECT::InsertToBuffer(OBJECT *where,int dir)
{
	ASSERT((dir == INSERT_HORIZ) || (dir == INSERT_VERT));

	if(where)
		this->LIST::Insert((LIST*)where,dir);
	else
		if(buffer)
			this->LIST::Insert((LIST*)buffer,dir);
		else
			buffer = this;
}

/*************
 * DESCRIPTION:   Frees all objects (main and paste buffer)
 * INPUT:         none
 * OUTPUT:        none
 *************/
void OBJECT::FreeObjectsList()
{
	OBJECT *next,*down,*obj;

	obj = this;
	while(obj)
	{
		next = (OBJECT*)obj->GetNext();
		down = (OBJECT*)obj->GoDown();

		delete obj;

		if(down)
			down->FreeObjectsList();

		obj = next;
	}
}

void FreeObjects()
{
	if(root)
		root->FreeObjectsList();
	root = NULL;
	if(buffer)
		buffer->FreeObjectsList();
	buffer = NULL;
	if(objerror)
	{
		delete objerror;
		objerror = NULL;
	}
	NoFirstSelected();
	cur_selected = NULL;
}

/*************
 * DESCRIPTION:   Set the color with wich the object is drawn
 * INPUT:         -
 * OUTPUT:        -
 *************/
void OBJECT::SetDrawColor(DISPLAY *display)
{
	if(surf)
	{
		if(display->usesurfacecolors)
		{
#ifdef __OPENGL__
			if(display->display == DISPLAY_SOLID)
			{
				float color[4];
				color[3] = 1.f;
				color[0] = surf->ambient.r * global.ambient.r;
				color[1] = surf->ambient.g * global.ambient.g;
				color[2] = surf->ambient.b * global.ambient.b;
				PPC_STUB(glMaterialfv)(GL_FRONT_AND_BACK, GL_AMBIENT, (GLfloat*)color);
				color[0] = surf->diffuse.r;
				color[1] = surf->diffuse.g;
				color[2] = surf->diffuse.b;
				PPC_STUB(glMaterialfv)(GL_FRONT_AND_BACK, GL_DIFFUSE, (GLfloat*)color);
				color[0] = surf->specular.r;
				color[1] = surf->specular.g;
				color[2] = surf->specular.b;
				PPC_STUB(glMaterialfv)(GL_FRONT_AND_BACK, GL_SPECULAR, (GLfloat*)color);
				PPC_STUB(glMaterialf)(GL_FRONT_AND_BACK, GL_SHININESS, surf->refphong);
			}
			else
#endif // __OPENGL_
				gfx.SetColor(surf->diffuse.r, surf->diffuse.g, surf->diffuse.b);
		}
		else
		{
#ifdef __OPENGL__
			if(display->display == DISPLAY_SOLID)
				SetGLColor(COLOR_SELECTED);
			else
#endif // __OPENGL__
			{
				if(selected)
					gfx.SetPen(COLOR_SELECTED);
				else
					gfx.SetPen(COLOR_UNSELECTED);
			}
		}
	}
	else
	{
#ifdef __OPENGL__
		if(display->display == DISPLAY_SOLID)
			SetGLColor(COLOR_SELECTED);
		else
#endif // __OPENGL__
		{
			if(selected)
				gfx.SetPen(COLOR_SELECTED);
			else
				gfx.SetPen(COLOR_UNSELECTED);
		}
	}
}

/*************
 * DESCRIPTION:   Draw selected object (objects which are out of screen are ignored)
 * INPUT:         display     current display
 *                stack       matrix stack
 *                root_pos    position of father's origin
 * OUTPUT:        -
 *************/
void OBJECT::DrawSelected(DISPLAY *display, MATRIX_STACK *stack, VECTOR *root_pos)
{
	OBJECT *help;
	VECTOR p;
	MATRIX m;

	ASSERT(display);
	ASSERT(stack);

	if((display->view->viewmode == VIEW_CAMERA) && (display->camera == (CAMERA*)this))
		return;

	if (selected)
	{
		if(IsVisible(display))
		{
			// draw line to parent object
#ifdef __OPENGL__
			if(root_pos && (display->display != DISPLAY_SOLID))
#else
			if(root_pos)
#endif // __OPENGL__
			{
				SetVector(&p, 0.f, 0.f, 0.f);
				stack->MultVectStack(&p);
				gfx.SetPen(COLOR_SELECTED);
				display->DrawLine(&p,root_pos);
			}
			if(!display->OutOfScreen(stack,&bboxmin,&bboxmax))
			{
				SetDrawColor(display);
#ifdef __OPENGL__
				if(display->display == DISPLAY_SOLID)
				{
					PPC_STUB(glPushMatrix)();
					OGLLoadMatrix(stack->GetMatrix());
					Draw(display,stack);
					display->DrawSelBox(stack, &bboxmin, &bboxmax);
					PPC_STUB(glPopMatrix)();
				}
				else
#endif // __OPENGL__
				{
					Draw(display,stack);

					if(external != EXTERNAL_CHILD)
						display->DrawAxis(stack, AXIS_SELECTED, &size);
					if(display->name_disp)
						display->DrawName(stack,GetName());
					display->DrawSelBox(stack, &bboxmin, &bboxmax);
					display->AccumulateBoundingRect(this,stack);
				}
			}
		}
	}
	// display brushes for this object
	help = (OBJECT*)brush;
	if(help)
	{
		SetVector(&p, 0.f, 0.f, 0.f);
		stack->MultVectStack(&p);
	}
	while(help)
	{
		if(help->selected && help->IsVisible(display))
		{
			m.SetOTMatrix(&help->orient_x, &help->orient_y, &help->orient_z, &help->pos);
			stack->Push(&m);
			help->DrawSelected(display, stack, &p);
			stack->Pop();
		}
		help = (OBJECT*)help->GetNext();
	}
	// display textures for this object
	help = (OBJECT*)texture;
	if(help)
	{
		SetVector(&p, 0.f, 0.f, 0.f);
		stack->MultVectStack(&p);
	}
	while(help)
	{
		if(help->selected && help->IsVisible(display))
		{
			m.SetOTMatrix(&help->orient_x, &help->orient_y, &help->orient_z, &help->pos);
			stack->Push(&m);
			help->DrawSelected(display, stack, &p);
			stack->Pop();
		}
		help = (OBJECT*)help->GetNext();
	}
}

/*************
 * DESCRIPTION:   Draw all dirty marked objects (objects which are out of screen are ignored)
 * INPUT:         display     current display
 * OUTPUT:        none
 *************/
void OBJECT::DrawDirty(DISPLAY *display, MATRIX_STACK *stack, VECTOR *root_pos)
{
	OBJECT *help;
	VECTOR p;
	MATRIX m;

	if((display->view->viewmode == VIEW_CAMERA) && (display->camera == (CAMERA*)this))
		return;

	if(flags & OBJECT_DIRTY)
	{
		if(!display->OutOfScreen(stack, &bboxmin, &bboxmax) && IsVisible(display))
		{
			if(root_pos)
			{
				SetVector(&p, 0.f, 0.f, 0.f);
				stack->MultVectStack(&p);
				if(selected)
					gfx.SetPen(COLOR_BKGROUND);
				else
					gfx.SetPen(COLOR_SELECTED);
				display->DrawLine(&p, root_pos);
			}

			if(selected)
				gfx.SetPen(COLOR_BKGROUND);
			else
				SetDrawColor(display);

			Draw(display,stack);

			if(external != EXTERNAL_CHILD)
				display->DrawAxis(stack, selected ? AXIS_REMOVE : 0, &size);
			if(display->name_disp)
				display->DrawName(stack, GetName());

			display->AccumulateBoundingRect(this,stack);
		}
	}
	// display brushes for this object
	help = (OBJECT*)brush;
	if(help)
	{
		if(selected)
			gfx.SetPen(COLOR_BKGROUND);
		else
			gfx.SetPen(COLOR_BRUSH);
		SetVector(&p, 0.f, 0.f, 0.f);
		stack->MultVectStack(&p);
	}
	while(help)
	{
		if(help->IsVisible(display))
		{
			m.SetOTMatrix(&help->orient_x, &help->orient_y, &help->orient_z, &help->pos);
			stack->Push(&m);
			help->DrawDirty(display, stack, &p);
			stack->Pop();
		}
		help = (OBJECT*)help->GetNext();
	}
	// display textures for this object
	help = (OBJECT*)texture;
	if(help)
	{
		if(selected)
			gfx.SetPen(COLOR_BKGROUND);
		else
			gfx.SetPen(COLOR_TEXTURE);
		SetVector(&p, 0.f, 0.f, 0.f);
		stack->MultVectStack(&p);
	}
	while(help)
	{
		if(help->IsVisible(display))
		{
			m.SetOTMatrix(&help->orient_x, &help->orient_y, &help->orient_z, &help->pos);
			stack->Push(&m);
			help->DrawDirty(display, stack, &p);
			stack->Pop();
		}
		help = (OBJECT*)help->GetNext();
	}
}

/*************
 * DESCRIPTION:   Draw objects (objects which are out of screen are ignored)
 * INPUT:         display     current display
 *                stack       matrix stack
 * OUTPUT:        none
 *************/
BOOL OBJECT::DrawObsList(DISPLAY *display, int mode, MATRIX_STACK *stack, VECTOR *root_pos)
{
	VECTOR pos;
	MATRIX m;
	OBJECT *obj, *down;

	obj = this;
	while(obj)
	{
		if (TestBreaked()) // let's see if the user wants to cancel the operation
			return TRUE;

		m.SetOTMatrix(&obj->orient_x, &obj->orient_y, &obj->orient_z, &obj->pos);
		stack->Push(&m);

		switch (mode)
		{
			case REDRAW_SELECTED: obj->DrawSelected(display, stack, root_pos); break;
			case REDRAW_DIRTY   : obj->DrawDirty(display, stack, root_pos); break;
		}

		down = (OBJECT*)obj->GoDown();
		if(down)
		{
			SetVector(&pos, 0.f, 0.f, 0.f);
			stack->MultVectStack(&pos);

			if(down->DrawObsList(display, mode, stack, &pos))
				return TRUE;
		}

		stack->Pop();

		obj = (OBJECT*)obj->GetNext();
	}
	return FALSE;
}

void DrawObs(DISPLAY *display, int mode)
{
	MATRIX_STACK stack;

	if(root)
		root->DrawObsList(display, mode, &stack, NULL);
}

/*************
 * DESCRIPTION:   Draw all objects (objects which are out of screen are ignored)
 * INPUT:         display     current display
 * OUTPUT:        none
 *************/
void OBJECT::DrawObj(DISPLAY *display, MATRIX_STACK *stack, VECTOR *root_pos)
{
	BOOL out;
	VECTOR p;
	VIEW *view = display->view;

	// don't draw us if we are the active camera
	if(((view->viewmode == VIEW_CAMERA) && (display->camera == (CAMERA*)this)) || !IsVisible(display))
		return;

	if((display->editmode & EDIT_ACTIVE) && (display->editmode & EDIT_OBJECT))
	{
		if(selected)
		{
			if(display->display != DISPLAY_BBOX)
				out = display->OutOfScreen(stack,&bboxmin,&bboxmax);
			else
				out = FALSE;
			if(!out)
			{
				SetDrawColor(display);
#ifdef __OPENGL__
				if(display->display == DISPLAY_SOLID)
				{
					PPC_STUB(glPushMatrix)();
					OGLLoadMatrix(stack->GetMatrix());
					Draw(display,stack);
					PPC_STUB(glPopMatrix)();
				}
				else
#endif // __OPENGL__
				{
					Draw(display,stack);

					if(!(display->editmode & EDIT_MESH))
					{
						if(external != EXTERNAL_CHILD)
							display->DrawAxis(stack, AXIS_SELECTED, &size);
						if(display->name_disp)
							display->DrawName(stack,GetName());
					}
				}

				display->AccumulateBoundingRect(this,stack);
			}
		}
		else
		{
			if((view->viewmode == VIEW_CAMERA) && display->camera->selected)
			{
				if(display->display != DISPLAY_BBOX)
					out = display->OutOfScreen(stack,&bboxmin,&bboxmax);
				else
					out = FALSE;
				if(!out)
				{
					display->AccumulateBoundingRect(this,stack);

					SetDrawColor(display);
#ifdef __OPENGL__
					if(display->display == DISPLAY_SOLID)
					{
						PPC_STUB(glPushMatrix)();
						OGLLoadMatrix(stack->GetMatrix());
						Draw(display,stack);
						PPC_STUB(glPopMatrix)();
					}
					else
#endif // __OPENGL__
					{
						Draw(display,stack);

						if(external != EXTERNAL_CHILD)
							display->DrawAxis(stack, 0, &size);
						if(display->name_disp)
							display->DrawName(stack,GetName());
					}
				}
			}
#ifdef __OPENGL__
			else
			{
				if(display->display == DISPLAY_SOLID)
				{
					out = display->OutOfScreen(stack,&bboxmin,&bboxmax);
					if(!out)
					{
						SetDrawColor(display);
						PPC_STUB(glPushMatrix)();
						OGLLoadMatrix(stack->GetMatrix());
						Draw(display,stack);
						PPC_STUB(glPopMatrix)();

						display->AccumulateBoundingRect(this,stack);
					}
				}
			}
#endif // __OPENGL__
		}
	}
	else
	{
#ifdef __OPENGL__
		if(root_pos && (display->display != DISPLAY_SOLID))
#else
		if(root_pos)
#endif // __OPENGL__
		{
			SetVector(&p, 0.f, 0.f, 0.f);
			stack->MultVectStack(&p);
			display->DrawLine(&p,root_pos);
		}

		if(display->display != DISPLAY_BBOX)
			out = display->OutOfScreen(stack,&bboxmin,&bboxmax);
		else
			out = FALSE;
		if(!out)
		{
			SetDrawColor(display);
#ifdef __OPENGL__
			if(display->display == DISPLAY_SOLID)
			{
				PPC_STUB(glPushMatrix)();
				OGLLoadMatrix(stack->GetMatrix());
				Draw(display,stack);
				if(selected)
					display->DrawSelBox(stack, &bboxmin, &bboxmax);
				PPC_STUB(glPopMatrix)();
			}
			else
#endif // __OPENGL__
			{
				Draw(display,stack);

				if(selected)
					display->DrawSelBox(stack, &bboxmin, &bboxmax);

				if(((display->display != DISPLAY_BBOX) ||
					!(display->editmode & EDIT_ACTIVE)) && (external != EXTERNAL_CHILD))
				{
					display->DrawAxis(stack, selected ? AXIS_SELECTED : 0, &size);
				}
				if(display->name_disp)
					display->DrawName(stack, GetName());
			}
		}
	}
	flags &= ~OBJECT_DIRTY;
}

BOOL OBJECT::DrawList(DISPLAY *display, MATRIX_STACK *stack, VECTOR *root_pos, BOOL added, BOOL drawsel)
{
	OBJECT *obj,*down,*brush,*texture;
	MATRIX m;
	VECTOR pos;
	BOOL tracked, add = FALSE;
	VECTOR p;

	obj = this;
	while(obj)
	{
		if(!(display->editmode & EDIT_ACTIVE))
		{
			if(TestBreaked()) // let us see if the user wants to break us
				return TRUE;
		}

		if(drawsel && obj->selected && (display->editmode & EDIT_ACTIVE) && (display->editmode & EDIT_OBJECT))
		{
			if(display->view->viewmode == VIEW_CAMERA)
			{
				tracked = obj->UpdateTracking(NULL);
			}
			else
			{
				p = obj->pos;
				stack->MultVectStack(&p);
				tracked = obj->UpdateTracking(&p);
			}

			m.SetOTMatrix(&obj->orient_x,&obj->orient_y,&obj->orient_z,&obj->pos);
			stack->Push(&m);

			if(!added)
			{
				if(display->localpos)
				{
					if(tracked)
						m.SetSTMatrix(&display->size, &display->pos);
					else
						m.SetSOTMatrix(&display->size, &display->orient_x, &display->orient_y, &display->orient_z, &display->pos);
				}
				else
				{
					if(tracked)
					{
						m.SetSMatrix(&display->size);
					}
					else
					{
						MATRIX m1,m2,m3;

						m = *stack->GetMatrix();
						m.m[1] = 0.f;
						m.m[2] = 0.f;
						m.m[3] = 0.f;
						m1 = m;
						m.InvMat(&m2);
						m.SetOMatrix(&display->orient_x, &display->orient_y, &display->orient_z);
						m3.MultMat(&m, &m2);
						m2.MultMat(&m1, &m3);
						m1.SetSMatrix(&display->size);
						m.MultMat(&m1, &m2);
					}
				}
				stack->Push(&m);
				added = TRUE;
				add = TRUE;
			}

			obj->DrawObj(display,stack,root_pos);

		}
		else
		{
			m.SetOTMatrix(&obj->orient_x,&obj->orient_y,&obj->orient_z,&obj->pos);
			stack->Push(&m);

			if(drawsel == obj->selected)
				obj->DrawObj(display,stack,root_pos);
		}
		
		SetVector(&pos, 0.f, 0.f, 0.f);
		stack->MultVectStack(&pos);

		down = (OBJECT*)obj->GoDown();
		if(down)
		{
			if(down->DrawList(display,stack,&pos,added,drawsel))
				return TRUE;
		}

		// display brushes for this object
		brush = (OBJECT*)obj->brush;
		while(brush)
		{
			m.SetOTMatrix(&brush->orient_x,&brush->orient_y,&brush->orient_z,&brush->pos);
			stack->Push(&m);

			if(!obj->selected && brush->selected &&
				(display->editmode & EDIT_ACTIVE) && (display->editmode & EDIT_OBJECT))
			{
				if(display->localalign)
				  m.SetSOMatrix(&display->size,&display->orient_x,&display->orient_y,&display->orient_z);
				else
				  m.SetSMatrix(&display->size);
				stack->Push(&m);
				brush->DrawObj(display,stack,&pos);
				stack->Pop();
			}
			else
				brush->DrawObj(display,stack,&pos);

			stack->Pop();
			brush = (OBJECT*)brush->GetNext();
		}

		// display textures for this object
		texture = (OBJECT*)obj->texture;
		while(texture)
		{
			m.SetOTMatrix(&texture->orient_x,&texture->orient_y,&texture->orient_z,&texture->pos);
			stack->Push(&m);

			if(!obj->selected && texture->selected &&
				(display->editmode & EDIT_ACTIVE) && (display->editmode & EDIT_OBJECT))
			{
				if(display->localalign)
				  m.SetSOMatrix(&display->size,&display->orient_x,&display->orient_y,&display->orient_z);
				else
				  m.SetSMatrix(&display->size);
				stack->Push(&m);
				texture->DrawObj(display,stack,&pos);
				stack->Pop();
			}
			else
				texture->DrawObj(display,stack,&pos);

			stack->Pop();
			texture = (OBJECT*)texture->GetNext();
		}

		if(add)
		{
			stack->Pop();
			add = FALSE;
			added = FALSE;
		}

		stack->Pop();

		obj = (OBJECT*)obj->GetNext();
	}

	return FALSE;
}

void DrawObjects(DISPLAY *display, BOOL drawsel)
{
	MATRIX_STACK stack;
	MATRIX m;

	if(root)
	{
		// if we want to move the object in world axis directions we have to push the
		// translation matrix first
		if((display->editmode & EDIT_ACTIVE) && (display->editmode & EDIT_OBJECT) && drawsel)
		{
			if(!display->localpos)
			{
				m.SetTMatrix(&display->pos);
				stack.Push(&m);
			}
		}
		root->DrawList(display,&stack,NULL,FALSE,drawsel);
	}
}

/*************
 * DESCRIPTION:   test if there is a object at a given position
 * INPUT:         display     current display
 *                x,y         position to test
 *                multi       TRUE if multiselect enabled else FALSE
 * OUTPUT:        selected object
 *************/
static int num=0,curnum;
static BOOL first;
static OBJECT *firstobj = NULL;  // first object at position

void OBJECT::Select()
{
	OBJECT *brush,*texture;

	if(!selected)
	{
		selected = TRUE;
		flags |= OBJECT_DIRTY;

		// select all brushes
		brush = (OBJECT*)this->brush;
		while(brush)
		{
			if(!brush->selected)
			{
				brush->selected = TRUE;
				brush->flags |= OBJECT_DIRTY;
			}

			brush = (OBJECT*)brush->GetNext();
		}

		// select all textures
		texture = (OBJECT*)this->texture;
		while(texture)
		{
			if(!texture->selected)
			{
				texture->selected = TRUE;
				texture->flags |= OBJECT_DIRTY;
			}

			texture = (OBJECT*)texture->GetNext();
		}
	}
}

void OBJECT::SelectList()
{
	OBJECT *down,*obj;

	obj = this;
	while(obj)
	{
		obj->Select();

		down = (OBJECT*)obj->GoDown();
		if(down)
			down->SelectList();

		obj = (OBJECT*)obj->GetNext();
	}
}

void OBJECT::Deselect(OBJECT *cur, BOOL editpoints)
{
	OBJECT *brush,*texture;

	if(selected && this != cur)
	{
		selected = FALSE;
		flags |= OBJECT_DIRTY;
		if(((GetType() == OBJECT_MESH) || (GetType() == OBJECT_SOR)) && editpoints)
		{
			// free selected point buffers
			this->FreeSelectedPoints();
		}
	}

	// deselect brushes for this object
	brush = (OBJECT*)this->brush;
	while(brush)
	{
		if(brush->selected && brush != cur)
		{
			brush->selected = FALSE;
			brush->flags |= OBJECT_DIRTY;
		}

		brush = (OBJECT*)brush->GetNext();
	}

	// deselect textures for this object
	texture = (OBJECT*)this->texture;
	while(texture)
	{
		if(texture->selected && texture != cur)
		{
			texture->selected = FALSE;
			texture->flags |= OBJECT_DIRTY;
		}

		texture = (OBJECT*)texture->GetNext();
	}
}

void OBJECT::DeselectList(OBJECT *cur, BOOL editpoints)
{
	OBJECT *down,*obj;

	obj = this;
	while(obj)
	{
		obj->Deselect(cur, editpoints);

		if((obj != cur) || (obj->external != EXTERNAL_NONE))
		{
			down = (OBJECT*)obj->GoDown();
			if(down)
				down->DeselectList(cur, editpoints);
		}

		obj = (OBJECT*)obj->GetNext();
	}
}

OBJECT *OBJECT::TestSelected(DISPLAY *display,WORD x,WORD y,BOOL multi, MATRIX_STACK *stack)
{
	VECTOR v;
	OBJECT *down;

	v = pos;
	stack->MultVectStack(&v);
	display->view->MCtoDC_xy(&v);
	if (x < v.x + TRACKWIDTH && x > v.x - TRACKWIDTH && y > v.y - TRACKWIDTH && y < v.y + TRACKWIDTH)
	{
		if(!first)
		{
			first = TRUE;
			if(firstobj != this)
			{
				num = 0;
				firstobj = this;
			}
			else
				num++;
		}
		else
			curnum++;

		if(num == curnum)
		{
			// in EDIT_MESH-mode only one points object can be selected at once
			if((display->editmode & EDIT_MESH) && multi)
			{
				if((GetType() == OBJECT_MESH) || (GetType() == OBJECT_SOR))
					multi = FALSE;
				else
				{
					if(firstselected)
					{
						if((firstselected->GetType() == OBJECT_MESH) || (firstselected->GetType() == OBJECT_SOR))
							multi = FALSE;
					}
				}
			}

			// deselect all other objects if multiselect is off
			if(!multi)
				root->DeselectList(this, display->editmode & EDIT_MESH);

			if(!selected)
				Select();

			down = (OBJECT*)GoDown();
			// select all child objects
			if(down)
				down->SelectList();

			if(!multi)
			{
				IsFirstSelected();
				cur_selected = NULL;
			}
			else
			{
				if(!firstselected)
				{
					IsFirstSelected();
					cur_selected = NULL;
				}
			}

			return this;
		}
	}
	return NULL;
}

OBJECT *OBJECT::TestSelectedList(DISPLAY *display,WORD x,WORD y,BOOL multi, MATRIX_STACK *stack)
{
	OBJECT *selobj,*obj,*brush,*texture,*down;
	MATRIX m;

	obj = this;
	while(obj)
	{
		selobj = obj->TestSelected(display,x,y,multi,stack);
		if(selobj)
			return selobj;

		m.SetOTMatrix(&obj->orient_x,&obj->orient_y,&obj->orient_z,&obj->pos);
		stack->Push(&m);

		// test brushes for this object
		if(display->filter_flags & FILTER_BRUSH)
		{
			brush = (OBJECT*)obj->brush;
			while(brush)
			{
				selobj = brush->TestSelected(display,x,y,multi,stack);
				if(selobj)
					return selobj;

				brush = (OBJECT*)brush->GetNext();
			}
		}

		// test textures for this object
		if(display->filter_flags & FILTER_TEXTURE)
		{
			texture = (OBJECT*)obj->texture;
			while(texture)
			{
				selobj = texture->TestSelected(display,x,y,multi,stack);
				if(selobj)
					return selobj;

				texture = (OBJECT*)texture->GetNext();
			}
		}

		// only test objects which are no child of an external object
		if(obj->external != EXTERNAL_ROOT)
		{
			down = (OBJECT*)obj->GoDown();
			if(down)
			{
				selobj = down->TestSelectedList(display,x,y,multi,stack);
				if(selobj)
					return selobj;
			}
		}

		stack->Pop();

		obj = (OBJECT*)obj->GetNext();
	}

	return NULL;
}

OBJECT *TestSelected(DISPLAY *display,WORD x,WORD y,BOOL multi)
{
	MATRIX_STACK stack;
	OBJECT *selobj, *obj;

	first = FALSE;
	curnum = 0;

	if(root)
	{
		selobj = root->TestSelectedList(display,x,y,multi,&stack);

		if(!selobj)
		{
			if(curnum == 0)
			{
				if(!first)
				{
					// nothing at this position
					firstobj = NULL;
				}
				else
				{
					// only one object at this position -> return it
					if(!firstobj->selected)
					{
						curnum = 0;
						obj = firstobj;
						firstobj = NULL;
						first = FALSE;
						obj->TestSelected(display,x,y,multi,&stack);
					}
				}
				return firstobj;
			}
			else
			{
				// last object at this position -> loop around
				curnum = 0;
				obj = firstobj;
				firstobj = NULL;
				first = FALSE;
				// and select it
				return obj->TestSelected(display,x,y,multi,&stack);
			}
		}

		return selobj;
	}
	else
		return NULL;
}

/*************
 * DESCRIPTION:   Test if a point of a points object is selectable at this position
 * INPUT:         display     current display
 *                x,y         position to test
 *                multi       TRUE if multiselect enabled else FALSE
 * OUTPUT:        TRUE -> new point selected else FALSE
 *************/
BOOL OBJECT::TestSelectedPointsList(DISPLAY *display,WORD x,WORD y,BOOL multi, MATRIX_STACK *stack)
{
	OBJECT *obj,*down;
	BOOL sel;
	MATRIX m;

	obj = this;
	while(obj)
	{
		down = (OBJECT*)obj->GoDown();

		if(obj->selected && ((obj->GetType() == OBJECT_MESH) || (obj->GetType() == OBJECT_SOR)))
		{
			m.SetOTMatrix(&obj->orient_x,&obj->orient_y,&obj->orient_z,&obj->pos);
			stack->Push(&m);

			if(display->editmode & EDIT_EDITPOINTS)
				sel = obj->TestSelectedPoints(display, x, y, multi, stack);
			else
			{
				if(display->editmode & EDIT_EDITEDGES)
					sel = obj->TestSelectedEdges(display, x, y, multi, stack);
				else
					sel = obj->TestSelectedTrias(display, x, y, multi, stack);
			}
			if(sel)
			{
				obj->flags |= OBJECT_DIRTY;
				return sel;
			}
		}
		else
		{
			if(down)
			{
				m.SetOTMatrix(&obj->orient_x,&obj->orient_y,&obj->orient_z,&obj->pos);
				stack->Push(&m);
			}
		}

		// only test objects which are no child of an external object
		if(obj->external != EXTERNAL_ROOT)
		{
			if(down)
			{
				sel = down->TestSelectedPointsList(display,x,y,multi,stack);
				if(sel)
					return sel;
			}
		}

		if(down)
			stack->Pop();

		obj = (OBJECT*)obj->GetNext();
	}

	return NULL;
}

BOOL TestSelectedPoints(DISPLAY *display,WORD x,WORD y,BOOL multi)
{
	MATRIX_STACK stack;

	if(root)
		return root->TestSelectedPointsList(display,x,y,multi,&stack);

	return FALSE;
}

/*************
 * DESCRIPTION:   test if there are any objects in a given box
 * INPUT:         display     current display
 *                box         selection box
 *                multi       TRUE if multiselect enabled else FALSE
 * OUTPUT:        selected object
 *************/
OBJECT *OBJECT::TestSelectedBox(DISPLAY *display, RECTANGLE2D *box, BOOL multi, MATRIX_STACK *stack)
{
	VECTOR v;
	OBJECT *down;

	v = pos;
	stack->MultVectStack(&v);
	display->view->MCtoDC_xy(&v);
	if (v.x > box->left && v.x < box->right &&
		 v.y > box->top && v.y < box->bottom)
	{
		if(!selected)
			Select();

		down = (OBJECT*)GoDown();
		// select all child objects
		if(down)
			down->SelectList();

		if(!multi)
		{
			IsFirstSelected();
			cur_selected = NULL;
		}
		else
		{
			if(!firstselected)
			{
				IsFirstSelected();
				cur_selected = NULL;
			}
		}

		return this;
	}
	return NULL;
}

OBJECT *OBJECT::TestSelectedBoxList(DISPLAY *display, RECTANGLE2D *box, BOOL multi, MATRIX_STACK *stack)
{
	OBJECT *selobj=NULL, *selected=NULL, *obj, *brush, *texture, *down;
	MATRIX m;

	obj = this;
	while(obj)
	{
		selobj = obj->TestSelectedBox(display, box, multi, stack);
		if(selobj && !multi)
			return selobj;

		m.SetOTMatrix(&obj->orient_x,&obj->orient_y,&obj->orient_z,&obj->pos);
		stack->Push(&m);

		// test brushes for this object
		if(!selobj && (display->filter_flags & FILTER_BRUSH))
		{
			brush = (OBJECT*)obj->brush;
			while(brush)
			{
				selobj = brush->TestSelectedBox(display, box, multi, stack);
				if(selobj && !multi)
					return selobj;

				brush = (OBJECT*)brush->GetNext();
			}
		}

		// test textures for this object
		if(!selobj && (display->filter_flags & FILTER_TEXTURE))
		{
			texture = (OBJECT*)obj->texture;
			while(texture)
			{
				selobj = texture->TestSelectedBox(display, box, multi, stack);
				if(selobj && !multi)
					return selobj;

				texture = (OBJECT*)texture->GetNext();
			}
		}

		// only test objects which are no child of an external object
		if(!selobj && (obj->external != EXTERNAL_ROOT))
		{
			down = (OBJECT*)obj->GoDown();
			if(down)
			{
				selobj = down->TestSelectedBoxList(display, box, multi, stack);
				if(selobj && !multi)
					return selobj;
			}
		}

		if(selobj)
			selected = selobj;

		stack->Pop();

		obj = (OBJECT*)obj->GetNext();
	}
	return selected;
}

OBJECT *TestSelectedBox(DISPLAY *display, RECTANGLE2D *box, BOOL multi)
{
	MATRIX_STACK stack;

	// deselect all other objects if multiselect is off
	if(!multi)
		root->DeselectList(NULL, display->editmode & EDIT_MESH);

	NoFirstSelected();

	if(root)
	{
		root->TestSelectedBoxList(display, box, multi, &stack);
		return firstselected;
	}
	else
		return NULL;
}

/*************
 * DESCRIPTION:   deselect an object and it's childs
 * INPUT:         -
 * OUTPUT:        -
 *************/
void OBJECT::DeselectObject()
{
	OBJECT *down;

	Deselect(NULL, FALSE);
	down = (OBJECT*)GoDown();
	if(down)
		down->DeselectList(NULL, FALSE);
}

/*************
 * DESCRIPTION:   deselect all selected objects
 * INPUT:         -
 * OUTPUT:        -
 *************/
void DeselectAll()
{
	if(root)
		root->DeselectList(NULL, FALSE);
	NoFirstSelected();
}

/*************
 * DESCRIPTION:   select an object and it's childs
 * INPUT:         -
 * OUTPUT:        -
 *************/
void OBJECT::SelectObject()
{
	OBJECT *down;

	IsFirstSelected();

	Select();
	down = (OBJECT*)GoDown();
	if(down)
		down->SelectList();
}

/*************
 * DESCRIPTION:   select all unselected objects
 * INPUT:         -
 * OUTPUT:        -
 *************/
void SelectAll()
{
	if(root)
		root->SelectList();
}

/*************
 * DESCRIPTION:   select next object
 * INPUT:         -
 * OUTPUT:        -
 *************/
void SelectNext()
{
	if(!cur_selected)
	{
		if(firstselected)
			cur_selected = firstselected;
		else
			cur_selected = root;
	}

	cur_selected->DeselectObject();

	cur_selected = (OBJECT*)cur_selected->GetNext();
	if(!cur_selected)
		cur_selected = root;

	cur_selected->SelectObject();
}

/*************
 * DESCRIPTION:   select previous object
 * INPUT:         -
 * OUTPUT:        -
 *************/
void SelectPrev()
{
	OBJECT *next;

	if(!cur_selected)
	{
		if(firstselected)
			cur_selected = firstselected;
		else
			cur_selected = root;
	}

	cur_selected->DeselectObject();

	cur_selected = (OBJECT*)cur_selected->GetPrev();
	if(!cur_selected)
	{
		// skip to end
		cur_selected = root;
		next = (OBJECT*)cur_selected->GetNext();
		while(next)
		{
			cur_selected = next;
			next = (OBJECT*)cur_selected->GetNext();
		}
	}

	cur_selected->SelectObject();
}

/*************
 * DESCRIPTION:   deselect all brushes
 * INPUT:         -
 * OUTPUT:        -
 *************/
void OBJECT::DeselectBrushesList()
{
	OBJECT *down,*obj,*brush;

	obj = this;
	while(obj)
	{
		// deselect brushes for this object
		brush = (OBJECT*)obj->brush;
		while(brush)
		{
			if(brush->selected)
			{
				brush->selected = FALSE;
				brush->flags |= OBJECT_DIRTY;
			}
			brush = (OBJECT*)brush->GetNext();
		}

		down = (OBJECT*)obj->GoDown();
		if(down)
			down->DeselectBrushesList();

		obj = (OBJECT*)obj->GetNext();
	}
}

void DeselectBrushes()
{
	if(root)
		root->DeselectBrushesList();
}

/*************
 * DESCRIPTION:   deselect all textures
 * INP        -
 * OUTPUT:        -
 *************/
void OBJECT::DeselectTexturesList()
{
	OBJECT *down,*obj,*texture;

	obj = this;
	while(obj)
	{
		// deselect textures for this object
		texture = (OBJECT*)obj->texture;
		while(texture)
		{
			if(texture->selected)
			{
				texture->selected = FALSE;
				texture->flags |= OBJECT_DIRTY;
			}
			texture = (OBJECT*)texture->GetNext();
		}

		down = (OBJECT*)obj->GoDown();
		if(down)
			down->DeselectTexturesList();

		obj = (OBJECT*)obj->GetNext();
	}
}

void DeselectTextures()
{
	if(root)
		root->DeselectTexturesList();
}

/*************
 * DESCRIPTION:   update the tracking directions
 * INPUT:         -
 * OUTPUT:        -
 *************/
void OBJECT::UpdateTrackingList()
{
	OBJECT *down,*obj;

	obj = this;
	while(obj)
	{
		obj->UpdateTracking(&obj->pos);

		down = (OBJECT*)obj->GoDown();
		if(down)
			down->UpdateTrackingList();

		obj = (OBJECT*)obj->GetNext();
	}
}

/*************
 * DESCRIPTION:   update the tracking directions (stub function for UpdateTrackingList())
 * INPUT:         -
 * OUTPUT:        -
 *************/
void UpdateTracking()
{
	if(root)
		root->UpdateTrackingList();
}

/*************
 * DESCRIPTION:   set the positions, orientations and sizes of the active
 *    object(s)
 * INPUT:         disp     current display
 * OUTPUT:        -
 *************/
void OBJECT::SetActiveObject(DISPLAY *disp, MATRIX_STACK *stack, BOOL add)
{
	VECTOR ox,oy,oz, p;
	MATRIX m,m1,matrix;

	if(add)
	{
		if(disp->localpos)
		{
			// translation of an object in the local coordinate system of the object
			// Formula:
			//                -1
			// p' = p + d * ml
			//
			// with:
			// p' = new position (in coordinate system of parent object)
			// p  = old position (in coordinate system of parent object)
			// d  = translation vector (in local coordinate system of object)
			// ml = rotation matrix of current object

			// transform translation vector to coordinate system of the parent object
			p = disp->pos;
			m.SetOMatrix(&orient_x, &orient_y, &orient_z);
			m.MultDirMat(&p);
			// add transformed translate vector
			VecAdd(&pos, &p, &pos);
		}
		else
		{
			// translation of an object in the world coordinate system
			// Formula:
			//                -1
			// p' = p + d * mw
			//
			// with
			// p' = new position (in coordinate system of parent object)
			// p  = old position (in coordinate system of parent object)
			// d  = translation vector (in world coordinate system)
			// mw = rotation matrix to transform from coordinate system of the parent
			//      object to the world coordinate system

			// transform translate vector to local coordinate system
			p = disp->pos;
			stack->MultInvDirStack(&p);
			VecAdd(&pos, &p, &pos);
		}

		if(disp->localalign)
		{
			// rotation of an object in the local coordinate system of the object
			// Formula:
			//
			// on' = ron * on
			//
			// with:
			// n   = one of x,y,z
			// on' = new rotation vector
			// ron = rotation vector
			// on  = old rotation vector
			SetVector(&ox,
				dotp(&disp->orient_x, &orient_x),
				dotp(&disp->orient_y, &orient_x),
				dotp(&disp->orient_z, &orient_x));
			SetVector(&oy,
				dotp(&disp->orient_x, &orient_y),
				dotp(&disp->orient_y, &orient_y),
				dotp(&disp->orient_z, &orient_y));
			SetVector(&oz,
				dotp(&disp->orient_x, &orient_z),
				dotp(&disp->orient_y, &orient_z),
				dotp(&disp->orient_z, &orient_z));
			orient_x = ox;
			orient_y = oy;
			orient_z = oz;
		}
		else
		{
			// translation of an object in the world coordinate system
			// Formula:
			//
			//             -1
			// mo = mr * mw
			//
			// with
			// mo = new rotation matrix
			// mr = rotation matrix
			// mw = rotation matrix to transform from coordinate system of the parent
			//      object to the world coordinate system

			m.SetOMatrix(&disp->orient_x, &disp->orient_y, &disp->orient_z);
			stack->GetMatrix()->InvMat(&m1);
			matrix.MultMat(&m1, &m);
			SetVector(&orient_x, matrix.m[5], matrix.m[9], matrix.m[13]);
			SetVector(&orient_y, matrix.m[6], matrix.m[10], matrix.m[14]);
			SetVector(&orient_z, matrix.m[7], matrix.m[11], matrix.m[15]);
		}
	}
	else
	{
		pos.x *= disp->size.x;
		pos.y *= disp->size.y;
		pos.z *= disp->size.z;
	}

	size.x *= disp->size.x;
	size.y *= disp->size.y;
	size.z *= disp->size.z;
	bboxmin.x *= disp->size.x;
	bboxmin.y *= disp->size.y;
	bboxmin.z *= disp->size.z;
	bboxmax.x *= disp->size.x;
	bboxmax.y *= disp->size.y;
	bboxmax.z *= disp->size.z;
}

void OBJECT::SetActiveObjects(DISPLAY *disp, MATRIX_STACK *stack, OBJECT *up)
{
	if(!(disp->editmode & EDIT_MESH) || ((GetType() != OBJECT_MESH) && (GetType() != OBJECT_SOR)))
	{
		if(up)
		{
			if(!up->selected)
				SetActiveObject(disp, stack, TRUE);
			else
				SetActiveObject(disp, stack, FALSE);
		}
		else
			SetActiveObject(disp, stack, TRUE);
	}
	SetObject(disp, &disp->pos, &disp->orient_x, &disp->orient_y, &disp->orient_z, &disp->size);
}

void OBJECT::SetActiveObjectsList(DISPLAY *disp, MATRIX_STACK *stack, BOOL all, BOOL unsel)
{
	OBJECT *up,*down,*obj,*brush,*texture;
	MATRIX m;

	obj = this;
	up = (OBJECT*)obj->GoUp();
	while(obj)
	{
		m.SetOMatrix(&obj->orient_x, &obj->orient_y, &obj->orient_z);

		if(obj->selected || unsel)
			obj->SetActiveObjects(disp,stack,up);

		stack->Push(&m);

		// set brushes for this object
		brush = (OBJECT*)obj->brush;
		while(brush)
		{
			if(brush->selected || unsel)
				brush->SetActiveObjects(disp,stack,obj);

			brush = (OBJECT*)brush->GetNext();
		}

		// set textures for this object
		texture = (OBJECT*)obj->texture;
		while(texture)
		{
			if(texture->selected || unsel)
				texture->SetActiveObjects(disp, stack,obj);

			texture = (OBJECT*)texture->GetNext();
		}

		down = (OBJECT*)obj->GoDown();
		if(down)
			down->SetActiveObjectsList(disp, stack, TRUE, unsel);

		stack->Pop();

		if(all)
			obj = (OBJECT*)obj->GetNext();
		else
			obj = NULL;
	}
}

void SetActiveObjects(DISPLAY *disp)
{
	MATRIX_STACK stack;

	if(root)
	{
		// following problem:
		// each object has its orientation axis, but this is not identical with the visible
		// axis. We use the orientation to rotate the points, but we can't rotate the
		// orientation with the display orientation, because the object orientation
		// must be rotated to the mirrored direction. So we change the direction
		// of the display orientation with this two lines below here.
		// Exeption: if point editing mode is on.
		if(!(disp->editmode & EDIT_MESH))
		{
			if(disp->localalign)
				InvOrient(&disp->orient_x, &disp->orient_y, &disp->orient_z, &disp->orient_x, &disp->orient_y, &disp->orient_z);
		}

		root->SetActiveObjectsList(disp, &stack, TRUE, FALSE);
		root->UpdateTrackingList();
	}
}

void OBJECT::SetActiveObject(DISPLAY *disp)
{
	MATRIX_STACK stack;

	SetActiveObjectsList(disp, &stack, FALSE, TRUE);
	UpdateTrackingList();
}

/*************
 * DESCRIPTION:   set the sizes of the child of the object
 *    object(s)
 *                dsize       delta size
 * OUTPUT:        -
 *************/
void OBJECT::SetTreeSize(VECTOR *size)
{
	VECTOR pos;
	VECTOR orient_x, orient_y, orient_z;

	SetVector(&pos, 0.f, 0.f, 0.f);
	SetVector(&orient_x, 1.f, 0.f, 0.f);
	SetVector(&orient_y, 0.f, 1.f, 0.f);
	SetVector(&orient_z, 0.f, 0.f, 1.f);

	this->pos.x *= size->x;
	this->pos.y *= size->y;
	this->pos.z *= size->z;

	this->size.x *= size->x;
	this->size.y *= size->y;
	this->size.z *= size->z;
	bboxmin.x *= size->x;
	bboxmin.y *= size->y;
	bboxmin.z *= size->z;
	bboxmax.x *= size->x;
	bboxmax.y *= size->y;
	bboxmax.z *= size->z;

	SetObject(NULL, &pos, &orient_x, &orient_y, &orient_z, size);
}

void OBJECT::SetTreeSizesList(VECTOR *size)
{
	OBJECT *down,*obj,*brush,*texture;

	obj = this;
	while(obj)
	{
		obj->SetTreeSize(size);

		// set brushes for this object
		brush = (OBJECT*)obj->brush;
		while(brush)
		{
			brush->SetTreeSize(size);
			brush = (OBJECT*)brush->GetNext();
		}

		// set textures for this object
		texture = (OBJECT*)obj->texture;
		while(texture)
		{
			texture->SetTreeSize(size);
			texture = (OBJECT*)texture->GetNext();
		}

		down = (OBJECT*)obj->GoDown();
		if(down)
			down->SetTreeSizesList(size);

		obj = (OBJECT*)obj->GetNext();
	}
}

void OBJECT::SetTreeSizes(VECTOR *size)
{
	OBJECT *down;

	down = (OBJECT*)GoDown();
	if(down)
		down->SetTreeSizesList(size);
}

/*************
 * DESCRIPTION:   gets the active object(s)
 * INPUT:         selobj      selected object if only one object is selected
 *                withsurface only select objects with surfaces
 * OUTPUT:        TRUE if more than one object is selected else FALSE
 *************/
BOOL OBJECT::GetActiveObjs(OBJECT **selobj, BOOL withsurface)
{
	OBJECT *down,*obj,*brush,*texture;

	obj = this;
	while(obj)
	{
		if(obj->selected && (obj->surf || !withsurface))
		{
			if(*selobj && (obj != firstselected))
				return TRUE;
			else
				*selobj = obj;
		}

		// get brushes for this object
		if(!withsurface)
		{
			brush = (OBJECT*)obj->brush;
			while(brush)
			{
				if(brush->selected)
				{
					if(*selobj)
						return TRUE;
					else
						*selobj = (OBJECT*)brush;
				}

				brush = (OBJECT*)brush->GetNext();
			}

			// get textures for this object
			texture = (OBJECT*)obj->texture;
			while(texture)
			{
				if(texture->selected)
				{
					if(*selobj)
						return TRUE;
					else
						*selobj =  (OBJECT*)texture;
				}

				texture = (OBJECT*)texture->GetNext();
			}
		}

		if(obj->external != EXTERNAL_ROOT)
		{
			down = (OBJECT*)obj->GoDown();
			if(down)
			{
				if(down->GetActiveObjs(selobj, withsurface))
					return TRUE;
			}
		}

		obj = (OBJECT*)obj->GetNext();
	}

	return FALSE;
}

BOOL GetActiveObjects(OBJECT **selobj, BOOL withsurface)
{
	if(root)
	{
		if(firstselected)
		{
			if(firstselected->selected)
				*selobj = firstselected;
			else
				*selobj = NULL;
		}
		else
			*selobj = NULL;

		return root->GetActiveObjs(selobj, withsurface);
	}

	*selobj = NULL;
	return FALSE;
}

/*************
 * DESCRIPTION:   gets the popuplist for the selected object
 * INPUT:         -
 * OUTPUT:        popup list
 *************/
#ifdef __AMIGA__
struct NewMenu *OBJECT::GetPopUpList()
{
	return popupmenu;
}
#else
int OBJECT::GetResourceID()
{
	return IDR_POPUP_OBJECT;
}
#endif

/*************
 * DESCRIPTION:   loads an object
 * INPUT:         filename    object filename
 *                root        pointer to root object of loaded object tree
 *                            (if NULL the object is created and returned in root)
 * OUTPUT:        error string or NULL
 *************/
char *LoadObject(char* filename, MESH *root)
{
	FILE *typefile;
	FILE *objfile;
	char typebuf[18];
	char objbuf[16];
	char *errstr;
	char handlername[32];
#ifndef __AMIGA__
	char buf[256];
	HINSTANCE hLib;
	INITPROC objInit;
	READWRITEPROC objRead;
	CLEANUPPROC objCleanup;
#endif
	int i;
	BOOL identok=FALSE;
	UWORD mask;
	VECTOR pos = {0.,0.,0.};
	VECTOR scale = {1.,1.,1.};
	VECTOR ox = {1.,0.,0.};
	VECTOR oy = {0.,1.,0.};
	VECTOR oz = {0.,0.,1.};
	UNDO_CREATE *undo;
	ULONG version = 0;

	objfile = fopen(filename,"rb");
	if(!objfile)
		return errors[ERR_OPENOBJ];
	if(fread(objbuf,1,16,objfile) != 16)
	{
		fclose(objfile);
		return errors[ERR_READOBJ];
	}
	fclose(objfile);
#ifdef __AMIGA__
	// Amiga-version: get directory RayStorm was started from and
	// search file from there
	// get a lock to the program directory
	BPTR lock = GetProgramDir();
	// make program directory to current directory
	lock = CurrentDir(lock);
	typefile = fopen("modules/object/types","rb");
	// change current dir to old dir
	lock = CurrentDir(lock);
#else
	strcpy(buf, szWorkingDirectory);
	strcat(buf, "\\modules\\object");
	AddPart(buf, "types.dat", 256);
	typefile = fopen(buf, "rb");
#endif
	if(!typefile)
		return errors[ERR_OPENOBJTYPEFILE];

	while(!feof(typefile) && !identok)
	{
		// Read identification string
		// Format:
		// UWORD mask;    mask for bytes to test '1'->test; lowest bit -> first byte
		// UBYTE id[16];  bytes to compare
		// char name[8];  name of module
		if(fread(&mask,2,1,typefile) != 1)
		{
			fclose(typefile);
			return errors[ERR_READOBJTYPEFILE];
		}
#ifdef __SWAP__
		mask = SwapW(mask);
#endif
		if(fread(typebuf,1,16,typefile) != 16)
		{
			fclose(typefile);
			return errors[ERR_READOBJTYPEFILE];
		}

		// Compare first 16 bytes of object with identstring
		identok = TRUE;
		for(i=0; i<16; i++)
		{
			if(mask & 0x8000)
			{
				if(typebuf[i] != objbuf[i])
				{
					identok = FALSE;
					break;
				}
			}
			mask = mask << 1;
		}
		// Read object name (max. 8 chars)
		if(fread(typebuf,1,8,typefile) != 8)
		{
			fclose(typefile);
			return errors[ERR_READOBJTYPEFILE];
		}
	}
	if(identok)
	{
		fclose(typefile);
		typebuf[8] = 0;

		if(!root)
		{
			// create a new object and insert all objects after this
			root = new MESH;
			if(!root)
				return errors[ERR_MEM];
			root->surf = GetSurfaceByName(SURF_DEFAULTNAME);
			if(!root->surf)
				return errors[ERR_MEM];

			root->Append();

			undo = new UNDO_CREATE;
			if(undo)
			{
				if(undo->AddCreated(root))
					undo->Add();
				else
				{
					delete undo;
					undo = NULL;
				}
			}
		}
		else
		{
			scale = root->size;
			undo = NULL;
		}

		// copy filename
		if(!root->SetFileName(filename))
			return errors[ERR_MEM];

		root->external = EXTERNAL_ROOT;     // mark as external root object
		root->selected = TRUE;
		root->IsFirstSelected();

		if(!InitObjHand(root, INSERT_VERT, undo))
			return errors[ERR_MEM];

#ifdef __AMIGA__
		// open object handler library
		// make program directory to current directory
		lock = CurrentDir(lock);
		strcpy(handlername, "modules/object");
		AddPart(handlername, typebuf, 32);
#ifdef __MIXEDBINARY__
		strcat(handlername, "ppc");
#endif

		ObjHandBase = OpenLibrary(handlername, 2L);
		if(!ObjHandBase)
			return errors[ERR_WRONGOBJHANDLER];

		// change current dir to old dir
		CurrentDir(lock);
#else
		strcpy(buf, szWorkingDirectory);
		strcat(buf, "\\modules\\object");
		AddPart(buf, typebuf, 256);
		strcat(buf, ".dll");

		if (!(hLib = LoadLibrary(buf)))
			return errors[ERR_OBJHANDLER];
		if (!(objInit = (INITPROC)GetProcAddress(hLib, "objInit_")))
			return errors[ERR_OBJHANDLER];
		if (!(objRead = (READWRITEPROC)GetProcAddress(hLib, "objRead_")))
			return errors[ERR_OBJHANDLER];
		if (!(objCleanup = (CLEANUPPROC)GetProcAddress(hLib, "objCleanup_")))
			return errors[ERR_OBJHANDLER];
#endif

		// call handler init function
#ifdef __MIXEDBINARY__
		if(!ppc_objInit())
#else
		if(!objInit())
#endif // __MIXEDBINARY__
		{
#ifdef __AMIGA__
			CloseLibrary(ObjHandBase);
#else
			FreeLibrary(hLib);
#endif
			return errors[ERR_OBJHANDLER];
		}
		// delete last error
		if(objerror)
		{
			delete [ ] objerror;
			objerror = NULL;
		}

		// invoke handler
#ifdef __MIXEDBINARY__
		errstr = ppc_objRead(NULL, filename, &link, &pos, &ox, &oy, &oz, &scale, NULL, NULL, &version);
#else
		errstr = objRead(NULL, filename, &link, &pos, &ox, &oy, &oz, &scale, NULL, NULL, &version);
#endif // __MIXEDBINARY__
		if(errstr)
		{
			objerror = new char[strlen(errstr)+3];
			strcpy(objerror, errstr);
#ifdef __MIXEDBINARY__
			ppc_objCleanup();
#else
			objCleanup();
#endif // __MIXEDBINARY__
#ifdef __AMIGA__
			CloseLibrary(ObjHandBase);
#else
			FreeLibrary(hLib);
#endif
			return errors[ERR_OBJHANDLER];
		}

		// cleanup handler
#ifdef __MIXEDBINARY__
		ppc_objCleanup();
#else
		objCleanup();
#endif // __MIXEDBINARY__

		// free handler library
#ifdef __AMIGA__
		CloseLibrary(ObjHandBase);
#else
		FreeLibrary(hLib);
#endif

		CleanupObjHand();
		root->TranslateNamesList();
	}
	else
	{
		fclose(typefile);
		return errors[ERR_UNKNOWNOBJ];
	}
	return NULL;
}

/*************
 * DESCRIPTION:   saves the selected objects
 * INPUT:         filename    object filename
 * OUTPUT:        error string or NULL
 *************/
char *SaveObject(char* filename)
{
	struct IFFHandle *iff;
	ULONG vers;
	MESH *rootobj;
	OBJECT *prev;

	// Allocate IFF_File structure.
	iff = AllocIFF();
	if(!iff)
		return errors[ERR_MEM];

	// Set up IFF_File for AmigaDOS I/O.
#ifdef __AMIGA__
	iff->iff_Stream = Open(filename, MODE_NEWFILE);
#else
	iff->iff_Stream = Open_(filename, MODE_NEWFILE);
#endif
	if(!iff->iff_Stream)
	{
		IFFCleanup(iff);
		return errors[ERR_OPEN];
	}
	InitIFFasDOS(iff);

	// Start the IFF transaction.
	if(OpenIFF(iff, IFFF_WRITE))
	{
		IFFCleanup(iff);
		return errors[ERR_IFFPARSE];
	}

	PUSH_CHUNK(ID_RSOB);

	vers = RSOB_VERSION;
	if(!WriteLongChunk(iff, ID_VERS, &vers, 1))
		return FALSE;

	PUSH_CHUNK(ID_SRFS);

	if(!WriteSelectedSurfaces(iff))
	{
		IFFCleanup(iff);
		return errors[ERR_WRITE];
	}

	POP_CHUNK();

	if(!WriteObjects(iff, TRUE))
	{
		IFFCleanup(iff);
		return errors[ERR_WRITE];
	}

	POP_CHUNK();

	IFFCleanup(iff);

	rootobj = new MESH;
	if(!rootobj)
		return errors[ERR_MEM];
	rootobj->surf = new SURFACE;
	if(!rootobj->surf)
		return errors[ERR_MEM];

	prev = (OBJECT*)firstselected->GetPrev();
	rootobj->OBJECT::Insert(prev, INSERT_HORIZ);

	firstselected->LIST::DeChain((LIST**)&root);

	firstselected->AddTo((LIST*)rootobj);

	// copy filename
	if(!rootobj->SetFileName(filename))
		return errors[ERR_MEM];

	rootobj->external = EXTERNAL_ROOT;      // mark as external root object
	rootobj->selected = TRUE;

	return NULL;

}

/*************
 * DESCRIPTION:   preform tests for saving the selected objects
 * INPUT:         -
 * OUTPUT:        error string
 *************/
char *TestForSaveObject()
{
	OBJECT *obj;

	// test if selected object is on root level
	obj = root;
	while(obj && (obj != firstselected))
		obj = (OBJECT*)obj->GetNext();

	if(!obj)
		return errors[ERR_SAVEOBJNOSEL];

	if(obj->external != EXTERNAL_NONE)
		return errors[ERR_SAVEOBJNOSEL];

	return NULL;
}

/*************
 * DESCRIPTION:   returns a error string returned from the object handler
 *    or NULL if no error occured
 * INPUT:         none
 * OUTPUT:        error string or NULL
 *************/
char *GetLoadError()
{
	return objerror;
}

/*************
 * DESCRIPTION:   delete object
 * INPUT:         -
 * OUTPUT:        -
 *************/
void OBJECT::DeleteObject()
{
	if(!win && !opencnt)
	{
		if(istracked)
		{
			Untrack(this);
			istracked = FALSE;
		}

		this->Delete((LIST**)&root);
	}
}

/*************
 * DESCRIPTION:   delete selected objects
 * INPUT:         obj      object to exclude
 *                mode     mode (copy of display->editmode)
 * OUTPUT:        -
 *************/
void OBJECT::DeleteSelectedList(OBJECT *exclude, ULONG mode, UNDO_DELETE *undo)
{
	OBJECT *next,*down,*obj;
	BOOL done=FALSE;

	obj = this;
	while(obj)
	{
		next = (OBJECT*)obj->GetNext();

		// test if object is selected and avoid deleting of the active camera
		if(obj->selected && (obj != exclude) && !obj->win && !obj->opencnt)
		{
			if(mode & EDIT_EDITPOINTS)
				done = obj->DeleteSelectedPoints(undo);
			else
			{
				if(mode & EDIT_EDITEDGES)
					done = obj->DeleteSelectedEdges(undo);
				else
				{
					if(mode & EDIT_EDITFACES)
						done = obj->DeleteSelectedTrias(undo);
				}
			}

			if(!done)
			{
				if(obj->istracked)
				{
					Untrack(obj);
					obj->istracked = FALSE;
				}

				if(undo)
				{
					// move deleted object to undo buffer
					obj->DeChain((LIST**)&root);
					((LIST*)obj)->InitWithoutDown();

					undo->AddDeleted(obj);
				}
				else
				{
					obj->Delete((LIST**)&root);
				}
			}
			else
				done = FALSE;
		}
		else
		{
			down = (OBJECT*)obj->GoDown();
			if(down)
				down->DeleteSelectedList(exclude, mode, undo);
		}
		obj = next;
	}
}

void DeleteSelected(OBJECT *exclude, ULONG mode)
{
	UNDO_DELETE *undo;

	if(root)
	{
		undo = new UNDO_DELETE;

		root->DeleteSelectedList(exclude, mode, undo);
		if(!(mode & (EDIT_EDITPOINTS | EDIT_EDITEDGES | EDIT_EDITFACES)))
			NoFirstSelected();

		if(undo)
			undo->Add();
	}
}

/*************
 * DESCRIPTION:   Creates a unique name for an object.
 * INPUT:         name     base name
 * OUTPUT:        unique name
 *************/
BOOL OBJECT::SetName(char *name)
{
	int index = 0;
	char *buffer;
	OBJECT *obj;

	if(this->name)
	{
		delete this->name;
		this->name = NULL;
	}

	if(!name)
		return TRUE;

	buffer = new char[strlen(name)+10];
	if(!buffer)
		return FALSE;

	strcpy(buffer,name);

	obj = GetObjectByName(name);                 // search for an object with this name
	if(obj && obj != this)
	{                                            // found it
		do
		{
			index++;
			sprintf(buffer,"%s.%d",name,index);    // create a new name with an index
		}
		while(GetObjectByName(buffer));
	}

	this->name = new char[strlen(buffer)+1];
	if(!this->name)
	{
		delete buffer;
		return FALSE;
	}
	strcpy(this->name,buffer);
	delete buffer;

	return TRUE;
}

/*************
 * DESCRIPTION:   Search object by name return NULL if no object found
 * INPUT:         name     object name
 * OUTPUT:        object or NULL if none found
 *************/
OBJECT *OBJECT::GetObjByName(char *name)
{
	OBJECT *found,*down,*obj;

	obj = this;
	while(obj)
	{
		if(obj->name)
		{
			if(!strcmp(name,obj->name))
				return obj;
		}

		down = (OBJECT*)obj->GoDown();
		if(down)
		{
			found = down->GetObjByName(name);
			if(found)
				return found;
		}

		obj = (OBJECT*)obj->GetNext();
	}
	return NULL;
}

OBJECT *GetObjectByName(char *name)
{
	if(root)
		return root->GetObjByName(name);

	return NULL;
}

/*************
 * DESCRIPTION:   writes surfaces of selected objects to file
 * INPUT:         iff         iff file handle
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL OBJECT::WriteSelectedSurfacesList(struct IFFHandle *iff)
{
	OBJECT *obj,*down;

	obj = this;
	while(obj)
	{
		if(obj->surf)
		{
			if(!(obj->surf->flags & SURF_WRITTEN) && !(obj->surf->flags & SURF_DEFAULT))
			{
				if(!obj->surf->Write(iff))
					return FALSE;
				if(!(obj->surf->flags & SURF_EXTERNAL))
				{
					// remove material from material manager
					sciRemoveMaterial(obj->surf, FALSE);
				}
				obj->surf->flags |= SURF_WRITTEN | SURF_EXTERNAL;
			}
		}

		down = (OBJECT*)obj->GoDown();
		if(down)
		{
			if(!down->WriteSelectedSurfacesList(iff))
				return FALSE;
		}

		if(obj != firstselected)
			obj = (OBJECT*)obj->GetNext();
		else
			obj = NULL;
	}

	return TRUE;
}

BOOL WriteSelectedSurfaces(struct IFFHandle *iff)
{
	BOOL ret = FALSE;

	if(firstselected)
	{
		ret = firstselected->WriteSelectedSurfacesList(iff);
		SurfaceResetWritten();
	}
	return ret;
}

/*************
 * DESCRIPTION:   writes objects to file
 * INPUT:         iff         iff file handle
 *                write_sel   only write selected objects
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL OBJECT::WriteObj(struct IFFHandle *iff, BOOL write_sel)
{
	OBJECT *obj,*down;

	obj = this;
	while(obj)
	{
		if(!obj->WriteObject(iff))
			return FALSE;

		if(write_sel)
		{
			obj->external = EXTERNAL_CHILD;
			obj->flags |= OBJECT_DIRTY;
		}

		// don't save hierarchy of external objects
		if((obj->external == EXTERNAL_NONE) ||
			(((obj->external == EXTERNAL_ROOT) || (obj->external == EXTERNAL_CHILD)) && write_sel))
		{
			down = (OBJECT*)obj->GoDown();
			if(down)
			{
				// begin new hierarchy level
				if(PushChunk(iff, ID_HIER, ID_FORM, IFFSIZE_UNKNOWN))
					return FALSE;

				if(!down->WriteObj(iff,write_sel))
					return FALSE;

				if(PopChunk(iff))
					return FALSE;
			}
		}

		if(!write_sel || (obj != firstselected))
			obj = (OBJECT*)obj->GetNext();
		else
			obj = NULL;
	}

	return TRUE;
}

BOOL WriteObjects(struct IFFHandle *iff, BOOL write_sel)
{
	if(root)
	{
		if(write_sel)
		{
			if(firstselected)
				return firstselected->WriteObj(iff,write_sel);
		}
		else
			return root->WriteObj(iff,write_sel);
	}
	return FALSE;
}

/*************
 * DESCRIPTION:   writes the object axis to the scene file
 * INPUT:         iff      iff file handle
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL OBJECT::WriteCommon(struct IFFHandle *iff)
{
	VECTOR v[3];

	if(!WriteLongChunk(iff, ID_POSI, &pos, 3))
		return FALSE;

	v[0] = orient_x;
	v[1] = orient_y;
	v[2] = orient_z;
	if(!WriteLongChunk(iff, ID_ALGN, v, 9))
		return FALSE;

	if(!WriteLongChunk(iff, ID_SIZE, &size, 3))
		return FALSE;

	if(!WriteName(iff))
		return FALSE;

	if(track)
	{
		if(!WriteTrack(iff))
			return FALSE;
	}

	return WriteFlags(iff);
}

/*************
 * DESCRIPTION:   reads the object axis from the scene file
 * INPUT:         iff      iff file handle
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL OBJECT::ReadAxis(struct IFFHandle *iff)
{
	struct ContextNode *cn;
	long error = 0;

	while(!error)
	{
		error = ParseIFF(iff, IFFPARSE_RAWSTEP);
		if(error < 0 && error != IFFERR_EOC)
			return FALSE;

		// Get a pointer to the context node describing the current context
		cn = CurrentChunk(iff);
		if(!cn)
			continue;

		if(error == IFFERR_EOC)
		{
			error = 0;
			continue;
		}

		switch (cn->cn_ID)
		{
			case ID_POSI:
				if(!ReadFloat(iff,(float*)&pos,3))
					return FALSE;
				break;
			case ID_ALGN:
				if(!ReadFloat(iff,(float*)&orient_x,9))
					return FALSE;
				break;
			case ID_SIZE:
				if(!ReadFloat(iff,(float*)&size,3))
					return FALSE;
				bboxmax = size;
				bboxmin.x = -size.x;
				bboxmin.y = -size.y;
				bboxmin.z = -size.z;
				return TRUE;
			default:
				return FALSE;
		}
	}

	return TRUE;
}

/*************
 * DESCRIPTION:   writes the object surface to the scene file
 * INPUT:         iff      iff file handle
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL OBJECT::WriteSurface(struct IFFHandle *iff)
{
	BRUSH_OBJECT *brush;
	TEXTURE_OBJECT *texture;

	if(PushChunk(iff, ID_SURF, ID_FORM, IFFSIZE_UNKNOWN))
		return FALSE;
	if(!WriteChunk(iff, ID_NAME, surf->name, strlen(surf->name)+1))
		return FALSE;

	brush = this->brush;
	while(brush)
	{
		if(!brush->WriteObject(iff))
			return FALSE;
		brush = (BRUSH_OBJECT*)brush->GetNext();
	}

	texture = this->texture;
	while(texture)
	{
		if(!texture->WriteObject(iff))
			return FALSE;
		texture = (TEXTURE_OBJECT*)texture->GetNext();
	}

	if(PopChunk(iff))
		return FALSE;

	return TRUE;
}

/*************
 * DESCRIPTION:   writes the object flags to the scene file
 * INPUT:         iff      iff file handle
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL OBJECT::WriteFlags(struct IFFHandle *iff)
{
	ULONG f = 0;

	if(flags & OBJECT_NODEOPEN)
		f |= RSCN_OBJECT_NODEOPEN;
	if(flags & OBJECT_OPENTOP)
		f |= RSCN_OBJECT_OPENTOP;
	if(flags & OBJECT_OPENBOTTOM)
		f |= RSCN_OBJECT_OPENBOTTOM;
	if(flags & OBJECT_INVERTED)
		f |= RSCN_OBJECT_INVERTED;
	if(flags & OBJECT_NOPHONG)
		f |= RSCN_OBJECT_MESH_NOPHONG;
	if(flags & OBJECT_APPLYSURF)
		f |= RSCN_OBJECT_EXTERNAL_APPLY;
	if(flags & OBJECT_SOR_ACCURATE)
		f |= RSCN_OBJECT_SOR_ACCURATE;
	if(flags & OBJECT_LIGHT_SHADOW)
		f |= RSCN_OBJECT_LIGHT_SHADOW;
	if(flags & OBJECT_LIGHT_FLARES)
		f |= RSCN_OBJECT_LIGHT_FLARES;
	if(flags & OBJECT_LIGHT_STAR)
		f |= RSCN_OBJECT_LIGHT_STAR;
	if(flags & OBJECT_LIGHT_SHADOWMAP)
		f |= RSCN_OBJECT_LIGHT_SHADOWMAP;
	if(flags & OBJECT_LIGHT_TRACKFALLOFF)
		f |= RSCN_OBJECT_LIGHT_TRACKFALLOFF;
	if(flags & OBJECT_CAMERA_VFOV)
		f |= RSCN_OBJECT_CAMERA_VFOV;
	if(flags & OBJECT_CAMERA_FOCUSTRACK)
		f |= RSCN_OBJECT_CAMERA_FOCUSTRACK;
	if(flags & OBJECT_CAMERA_FASTDOF)
		f |= RSCN_OBJECT_CAMERA_FASTDOF;

	return WriteLongChunk(iff, ID_FLGS, &f, 1);
}

/*************
 * DESCRIPTION:   reads the object flags from the scene file
 * INPUT:         iff      iff file handle
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL OBJECT::ReadFlags(struct IFFHandle *iff)
{
	ULONG f;

	if(!ReadULONG(iff, &f,1))
		return FALSE;

	flags = 0;

	if(f & RSCN_OBJECT_NODEOPEN)
		flags |= OBJECT_NODEOPEN;
	if(f & RSCN_OBJECT_OPENTOP)
		flags |= OBJECT_OPENTOP;
	if(f & RSCN_OBJECT_OPENBOTTOM)
		flags |= OBJECT_OPENBOTTOM;
	if(f & RSCN_OBJECT_INVERTED)
		flags |= OBJECT_INVERTED;
	if(f & RSCN_OBJECT_MESH_NOPHONG)
		flags |= OBJECT_NOPHONG;
	if(f & RSCN_OBJECT_EXTERNAL_APPLY)
		flags |= OBJECT_APPLYSURF;
	if(f & RSCN_OBJECT_SOR_ACCURATE)
		flags |= OBJECT_SOR_ACCURATE;
	if(f & RSCN_OBJECT_LIGHT_SHADOW)
		flags |= OBJECT_LIGHT_SHADOW;
	if(f & RSCN_OBJECT_LIGHT_FLARES)
		flags |= OBJECT_LIGHT_FLARES;
	if(f & RSCN_OBJECT_LIGHT_STAR)
		flags |= OBJECT_LIGHT_STAR;
	if(f & RSCN_OBJECT_LIGHT_SHADOWMAP)
		flags |= OBJECT_LIGHT_SHADOWMAP;
	if(f & RSCN_OBJECT_LIGHT_TRACKFALLOFF)
		flags |= OBJECT_LIGHT_TRACKFALLOFF;
	if(f & RSCN_OBJECT_CAMERA_VFOV)
		flags |= OBJECT_CAMERA_VFOV;
	if(f & RSCN_OBJECT_CAMERA_FOCUSTRACK)
		flags |= OBJECT_CAMERA_FOCUSTRACK;
	if(f & RSCN_OBJECT_CAMERA_FASTDOF)
		flags |= OBJECT_CAMERA_FASTDOF;

	return TRUE;
}

/*************
 * DESCRIPTION:   wirtes the object track name to the scene file
 * INPUT:         iff      iff file handle
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL OBJECT::WriteTrack(struct IFFHandle *iff)
{
	if(track)
		return WriteChunk(iff, ID_TRCK, track->GetName(), strlen(track->GetName())+1);
	return TRUE;
}

/*************
 * DESCRIPTION:   reads the object track name from the scene file
 * INPUT:         iff      iff file handle
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL OBJECT::ReadTrack(struct IFFHandle *iff)
{
	char buffer[256];

	if(!ReadString(iff,buffer,256))
		return FALSE;

	track = (OBJECT*)new char[strlen(buffer)+1];
	if(!track)
		return FALSE;

	strcpy((char*)(track),buffer);
	return TRUE;
}

/*************
 * DESCRIPTION:   writes the object name to the scene file
 * INPUT:         iff      iff file handle
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL OBJECT::WriteName(struct IFFHandle *iff)
{
	return WriteChunk(iff, ID_NAME, GetName(), strlen(GetName())+1);
}

/*************
 * DESCRIPTION:   reads the object name from the scene file
 * INPUT:         iff      iff file handle
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL OBJECT::ReadName(struct IFFHandle *iff)
{
	char buffer[256];

	if(!ReadString(iff,buffer,256))
		return FALSE;
	return SetName(buffer);
}

/*************
 * DESCRIPTION:   goes through object tree and calls the callback function
 *    for each object in the list
 * INPUT:         func     callback function
 *                data     a pointer to a data element with wich the callback function is
 *    called as second argument (so you don't have to deal with global variables)
 *                flags    see GETOBJECT_??? in object.h
 * OUTPUT:        -
 *************/
void OBJECT::GetObjectsList(void (*func)(OBJECT*, void*), void *data, ULONG flags)
{
	OBJECT *obj,*down;
	BRUSH_OBJECT *brush;
	TEXTURE_OBJECT *texture;

	obj = this;
	while(obj)
	{
		func(obj, data);

		if(obj->brush && (flags & GETOBJECT_BRUSHES))
		{
			brush = obj->brush;
			while(brush)
			{
				func(brush, data);
				brush = (BRUSH_OBJECT*)brush->GetNext();
			}
			if(obj->brush)
				func(NULL, data);
		}

		if(obj->texture && (flags & GETOBJECT_TEXTURES))
		{
			texture = obj->texture;
			while(texture)
			{
				func(texture, data);
				texture = (TEXTURE_OBJECT*)texture->GetNext();
			}
			if(obj->texture)
				func(NULL, data);
		}

		down = (OBJECT*)obj->GoDown();
		if(down)
			down->GetObjectsList(func, data, flags);

		func(NULL, data);

		obj = (OBJECT*)obj->GetNext();
	}
}

void GetObjects(void (*func)(OBJECT*, void *), void *data, ULONG flags)
{
	if(root)
		root->GetObjectsList(func, data, flags);
}

/*************
 * DESCRIPTION:   same as GetObjects() but for selected objects
 * INPUT:         func     callback function
 *                data     see GetObjects()
 *                flags    see GETOBJECT_??? in object.h
 * OUTPUT:        -
 *************/
void OBJECT::GetSelectedObjectsList(void (*func)(OBJECT*, void*), void *data, ULONG flags)
{
	OBJECT *obj,*down;
	BRUSH_OBJECT *brush;
	TEXTURE_OBJECT *texture;

	obj = this;
	while(obj)
	{
		if(obj->selected)
			func(obj, data);
		else
		{
			if(obj->brush && (flags & GETOBJECT_BRUSHES))
			{
				brush = obj->brush;
				while(brush)
				{
					if(brush->selected)
						func(brush, data);
					brush = (BRUSH_OBJECT*)brush->GetNext();
				}
				func(NULL, data);
			}

			if(obj->texture && (flags & GETOBJECT_TEXTURES))
			{
				texture = obj->texture;
				while(texture)
				{
					if(texture->selected)
						func(texture, data);
					texture = (TEXTURE_OBJECT*)texture->GetNext();
				}
				func(NULL, data);
			}

			down = (OBJECT*)obj->GoDown();
			if(down)
			{
				down->GetSelectedObjectsList(func, data, flags);
				func(NULL, data);
			}
		}

		obj = (OBJECT*)obj->GetNext();
	}
}

void GetSelectedObjects(void (*func)(OBJECT*, void *), void *data, ULONG flags)
{
	if(root)
		root->GetSelectedObjectsList(func, data, flags);
}

/*************
 * DESCRIPTION:   transfer scene data to RayStorm Interface
 * INPUT:         stack       matrix stack
 *                camera      active camera
 * OUTPUT:        rsiERR_NONE if ok else error number
 *************/
rsiResult OBJECT::ToRSIList(rsiCONTEXT *rc, MATRIX_STACK *stack, OBJECT *camera)
{
	rsiResult err;
	OBJECT *obj, *down;
	MATRIX m;
	void *object;

	obj = this;
	while(obj)
	{
		if(((obj->GetType() != OBJECT_CAMERA) || (obj == camera)) && (obj->external != EXTERNAL_CHILD))
		{
			m.SetOTMatrix(&obj->orient_x,&obj->orient_y,&obj->orient_z,&obj->pos);
			stack->Push(&m);

			err = obj->ToRSI(rc, stack, &object);
			if(err)
				return err;

			if((obj->external != EXTERNAL_ROOT) && (obj->GetType() != OBJECT_CSG))
			{
				down = (OBJECT*)obj->GoDown();
				if(down)
				{
					err = down->ToRSIList(rc, stack,camera);
					if(err)
						return err;
				}
			}

			stack->Pop();
		}

		obj = (OBJECT*)obj->GetNext();
	}
	return rsiERR_NONE;
}

rsiResult ToRSI(rsiCONTEXT *rc, OBJECT *camera)
{
	MATRIX_STACK stack;

	if(root)
		return root->ToRSIList(rc, &stack,camera);
	return rsiERR_NONE;
}

/*************
 * DESCRIPTION:   move selected objects to paste buffer
 * INPUT:         exclude     object to exclude
 * OUTPUT:        TRUE if all went ok else FALSE
 *************/
BOOL OBJECT::CutList(OBJECT *exclude, OBJECT *where, int dir, UNDO_DELETE *undo)
{
	OBJECT *obj,*next,*down, *dup;
	VECTOR pos, ox, oy, oz;
	MATRIX matrix;

	obj = this;
	while(obj)
	{
		next = (OBJECT*)obj->GetNext();
		down = (OBJECT*)obj->GoDown();

		if(obj->selected && !obj->win && !obj->opencnt)
		{
			if(obj != exclude)
			{
				Untrack(obj);

				// get absolute axis for objects on root level
				if(!where)
				{
					obj->GetObjectMatrix(&matrix);
					matrix.GenerateAxis(&ox, &oy, &oz, &pos);
				}
				obj->DeChain((LIST**)&root);
				((LIST*)obj)->InitWithoutDown();

				if(undo)
				{
					if(!undo->AddCutted(obj))
					{
						// not enough memory -> this step can't be undone
						delete undo;
						undo = NULL;
					}
				}

				if(!where)
				{
					obj->pos = pos;
					obj->orient_x = ox;
					obj->orient_y = oy;
					obj->orient_z = oz;
				}
				obj->InsertToBuffer(where,dir);
				where = obj;
			}
			else
			{
				dup = obj->Duplicate();
				if(!dup)
					return FALSE;
				((LIST*)dup)->InitWithoutDown();
				// get absolute axis for objects on root level
				if(!where)
				{
					obj->GetObjectMatrix(&matrix);
					matrix.GenerateAxis(&dup->orient_x, &dup->orient_y, &dup->orient_z, &dup->pos);
				}
				dup->InsertToBuffer(where,dir);
				where = dup;
			}
		}
		else
		{
			if(down)
			{
				if(!down->CutList(exclude, where,INSERT_VERT, undo))
					return FALSE;
			}
		}

		obj = next;
		dir = INSERT_HORIZ;
	}
	return TRUE;
}

BOOL Cut(OBJECT *exclude)
{
	UNDO_DELETE *undo;

	if(buffer)
	{
		buffer->FreeObjectsList();
		buffer = NULL;
	}
	if(root)
	{
		undo = new UNDO_DELETE;

		if(root->CutList(exclude, NULL, INSERT_HORIZ, undo))
		{
			if(undo)
				undo->Add();
		}
		else
			return FALSE;

	}
	return TRUE;
}

/*************
 * DESCRIPTION:   copy selected objects to paste buffer
 * INPUT:         -
 * OUTPUT:        TRUE if all went ok else FALSE
 *************/
BOOL OBJECT::CopyList(OBJECT *where, int dir)
{
	OBJECT *obj,*down,*dup;
	MATRIX matrix;

	obj = this;
	while(obj)
	{
		if(obj->selected)
		{
			dup = obj->Duplicate();
			if(!dup)
				return FALSE;
			// get absolute axis for objects on root level
			if(!where)
			{
				obj->GetObjectMatrix(&matrix);
				matrix.GenerateAxis(&dup->orient_x, &dup->orient_y, &dup->orient_z, &dup->pos);
			}
			dup->InsertToBuffer(where, dir);
			where = dup;
		}

		down = (OBJECT*)obj->GoDown();
		if(down)
		{
			if(!down->CopyList(where,INSERT_VERT))
				return FALSE;
		}

		obj = (OBJECT*)obj->GetNext();
		dir = INSERT_HORIZ;
	}
	return TRUE;
}

BOOL Copy()
{
	if(buffer)
	{
		buffer->FreeObjectsList();
		buffer = NULL;
	}
	if(root)
		return root->CopyList(NULL,INSERT_HORIZ);
	return TRUE;
}

/*************
 * DESCRIPTION:   paste from buffer to object list
 * INPUT:         display     current display
 * OUTPUT:        TRUE if all went ok else FALSE
 *************/
BOOL OBJECT::PasteList(OBJECT *where, int dir)
{
	OBJECT *obj,*down,*dup;

	obj = this;
	while(obj)
	{
		// duplicte object from paste buffer
		dup = obj->Duplicate();
		if(!dup)
			return FALSE;

		// the object has to be redrawn
		dup->flags |= OBJECT_DIRTY;

		// then we need a new name for this object
		if(!dup->SetName(obj->name))
		{
			delete dup;
			return FALSE;
		}

		// and now we insert it to the object tree
		dup->Insert(where, dir);
		where = dup;
		if(!firstselected)
			dup->IsFirstSelected();

		down = (OBJECT*)obj->GoDown();
		if(down)
		{
			if(!down->PasteList(where,INSERT_VERT))
				return FALSE;
		}

		obj = (OBJECT*)obj->GetNext();
		dir = INSERT_HORIZ;
	}
	return TRUE;
}

BOOL Paste()
{
	if(buffer)
	{
		// first deselect all objects
		DeselectAll();

		// then paste buffer
		return buffer->PasteList(root,INSERT_HORIZ);
	}
	return TRUE;
}

/*************
 * DESCRIPTION:   duplicate an object with all contents (surfaces, brushes, textures, ...)
 * INPUT:         where    where to insert next object after
 *                dir      insert direction
 * OUTPUT:        TRUE if all went ok else FALSE
 *************/
OBJECT *OBJECT::Duplicate()
{
	OBJECT *dup;

	dup = DupObj();
	if(!dup)
		return NULL;

	// set all connections to NULL
	((LIST*)dup)->Init();

	// we need this to avoid freeing the surface if something below will fail and we
	// have to delete the new object
	dup->surf = NULL;

	dup->name = new char[strlen(name)+1];
	if(!dup->name)
	{
		delete dup;
		return NULL;
	}
	strcpy(dup->name,name);

	// copy the surface (objects like camera and lights do not have a surface)
	if(surf)
	{
		dup->surf = surf->Duplicate();
		if(!dup->surf)
		{
			delete dup;
			return NULL;
		}
	}

	return dup;
}

/*************
 * DESCRIPTION:   get the bounding box for all or only for selected objects
 * INPUT:         min,max  bounding box
 *                all      if TRUE test all objects else test only selected objects
 * OUTPUT:        TRUE if bounding box calculated else FALSE (no object selected)
 *************/
void OBJECT::GetBBox(VECTOR *min, VECTOR *max, MATRIX_STACK *stack)
{
	VECTOR v;

	v = bboxmin;
	stack->MultVectStack(&v);
	if(v.x < min->x)
		min->x = v.x;
	if(v.y < min->y)
		min->y = v.y;
	if(v.z < min->z)
		min->z = v.z;

	v = bboxmax;
	stack->MultVectStack(&v);
	if(v.x > max->x)
		max->x = v.x;
	if(v.y > max->y)
		max->y = v.y;
	if(v.z > max->z)
		max->z = v.z;
}

void OBJECT::GetBBoxList(VECTOR *min, VECTOR *max, BOOL all, BOOL *found, MATRIX_STACK *stack)
{
	OBJECT *obj,*down, *brush, *texture;
	MATRIX m;

	obj = this;
	while(obj)
	{
		m.SetOTMatrix(&obj->orient_x,&obj->orient_y,&obj->orient_z,&obj->pos);
		stack->Push(&m);

		if(obj->selected || all)
		{
			obj->GetBBox(min,max,stack);
			*found = TRUE;
		}

		brush = (OBJECT*)obj->brush;
		while(brush)
		{
			if(brush->selected || all)
			{
				m.SetOTMatrix(&brush->orient_x,&brush->orient_y,&brush->orient_z,&brush->pos);
				stack->Push(&m);
				brush->GetBBox(min,max,stack);
				stack->Pop();
				*found = TRUE;
			}
			brush = (OBJECT*)brush->GetNext();
		}

		texture = (OBJECT*)obj->texture;
		while(texture)
		{
			if(texture->selected || all)
			{
				m.SetOTMatrix(&texture->orient_x,&texture->orient_y,&texture->orient_z,&texture->pos);
				stack->Push(&m);
				texture->GetBBox(min,max,stack);
				stack->Pop();
				*found = TRUE;
			}
			texture = (OBJECT*)texture->GetNext();
		}

		down = (OBJECT*)obj->GoDown();
		if(down)
			down->GetBBoxList(min, max, all, found, stack);

		stack->Pop();

		obj = (OBJECT*)obj->GetNext();
	}
}

BOOL GetBBox(VECTOR *min, VECTOR *max, BOOL all)
{
	MATRIX_STACK stack;
	BOOL found = FALSE;

	if(root)
	{
		min->x = min->y = min->z = INFINITY;
		max->x = max->y = max->z = -INFINITY;

		root->GetBBoxList(min, max, all, &found, &stack);
	}
	return found;
}

/*************
 * DESCRIPTION:   get the object matrix
 * INPUT:         matrix
 * OUTPUT:        TRUE -> found; else FALSE
 *************/
BOOL OBJECT::GetObjectMatrixList(OBJECT *which, MATRIX *matrix, MATRIX_STACK *stack)
{
	OBJECT *obj, *down, *brush, *texture;
	MATRIX m;

	obj = this;
	while(obj)
	{
		m.SetOTMatrix(&obj->orient_x, &obj->orient_y, &obj->orient_z, &obj->pos);
		stack->Push(&m);

		if(obj == which)
		{
			*matrix = *stack->GetMatrix();
			return TRUE;
		}

		brush = (OBJECT*)obj->brush;
		while(brush)
		{
			if(brush == which)
			{
				m.SetOTMatrix(&brush->orient_x, &brush->orient_y, &brush->orient_z, &brush->pos);
				stack->Push(&m);

				*matrix = *stack->GetMatrix();
				return TRUE;
			}
			brush = (OBJECT*)brush->GetNext();
		}

		texture = (OBJECT*)obj->texture;
		while(texture)
		{
			if(texture == which)
			{
				m.SetOTMatrix(&texture->orient_x, &texture->orient_y, &texture->orient_z, &texture->pos);
				stack->Push(&m);

				*matrix = *stack->GetMatrix();
				return TRUE;
			}
			texture = (OBJECT*)texture->GetNext();
		}

		down = (OBJECT*)obj->GoDown();
		if(down)
		{
			if(down->GetObjectMatrixList(which, matrix, stack))
				return TRUE;
		}

		stack->Pop();

		obj = (OBJECT*)obj->GetNext();
	}

	return FALSE;
}

BOOL OBJECT::GetObjectMatrix(MATRIX *matrix)
{
	MATRIX_STACK stack;

	if(root)
		return root->GetObjectMatrixList(this, matrix, &stack);

	return FALSE;
}

/*************
 * DESCRIPTION:   makes this object the first selected
 * INPUT:         -
 * OUTPUT:        -
 *************/
void OBJECT::IsFirstSelected()
{
	firstselected = this;

	// the toolbar buttons for mesh editing are
	// enabled only if a mesh or SOR object is selected
	if(firstselected->external == EXTERNAL_NONE)
	{
		switch(firstselected->GetType())
		{
			case OBJECT_MESH:
				utility.EnableToolbarItem(TOOLBAR_EDITMESH, TRUE);
				break;
			case OBJECT_SOR:
				utility.EnableToolbarItem(TOOLBAR_EDITSOR, TRUE);
				break;
			default:
				utility.EnableToolbarItem(TOOLBAR_EDITMESH, FALSE);
				break;
		}
	}
	else
		utility.EnableToolbarItem(TOOLBAR_EDITMESH, FALSE);
}

/*************
 * DESCRIPTION:   no first selected object
 * INPUT:         -
 * OUTPUT:        -
 *************/
void NoFirstSelected()
{
	firstselected = NULL;

	// disbale the toolbar buttons for mesh editing
	utility.EnableToolbarItem(TOOLBAR_EDITMESH, FALSE);
}

/*************
 * DESCRIPTION:   group selected objects
 * INPUT:         -
 * OUTPUT:        TRUE -> no problem, else FALSE
 *************/
void OBJECT::GroupObjectsList(OBJECT *group_to, VECTOR *p, MATRIX *m)
{
	OBJECT *obj, *next;
	MATRIX m1, matrix;

	obj = this;
	while(obj)
	{
		next = (OBJECT*)obj->GetNext();
		if(obj->selected && (obj != group_to))
		{
			// transform global axis to local axis
			VecSub(&obj->pos, p, &obj->pos);
			m1.SetOTMatrix(&obj->orient_x,&obj->orient_y,&obj->orient_z, &obj->pos);
			matrix.MultMat(&m1, m);
			SetVector(&obj->pos, matrix.m[1], matrix.m[2], matrix.m[3]);
			SetVector(&obj->orient_x, matrix.m[5], matrix.m[9], matrix.m[13]);
			SetVector(&obj->orient_y, matrix.m[6], matrix.m[10], matrix.m[14]);
			SetVector(&obj->orient_z, matrix.m[7], matrix.m[11], matrix.m[15]);

			// remove the object from the current position
			obj->DeChain((LIST**)&root);
			// and add it to the group root object
			obj->AddTo(group_to);

			obj->flags |= OBJECT_DIRTY;
		}

		obj = next;
	}
}

BOOL GroupObjects()
{
	OBJECT *obj;
	VECTOR p,ox,oy,oz;
	MATRIX m;

	if(root)
	{
		GetActiveObjects(&obj);
		if(obj)
		{
			obj->GetObjectMatrix(&m);
			m.GenerateAxis(&ox,&oy,&oz, &p);

			InvOrient(&ox, &oy, &oz, &ox, &oy, &oz);
			m.SetOMatrix(&ox, &oy, &oz);

			obj->flags |= OBJECT_DIRTY;
			root->GroupObjectsList(obj, &p, &m);
			return TRUE;
		}
	}
	return FALSE;
}

/*************
 * DESCRIPTION:   ungroup selected objects
 * INPUT:         -
 * OUTPUT:        -
 *************/
void OBJECT::UnGroupObjectsList()
{
	OBJECT *obj, *n, *next, *child;
	MATRIX matrix;

	n = (OBJECT*)GetNext();

	if(selected)
	{
		if(external != EXTERNAL_ROOT)
		{
			child = (OBJECT*)GoDown();
			while(child)
			{
				next = (OBJECT*)child->GetNext();

				// get global axis
				child->GetObjectMatrix(&matrix);

				// remove the object from the current position
				child->DeChain((LIST**)&root);
				child->LIST::InitWithoutDown();

				// set axis
				matrix.GenerateAxis(&child->orient_x,&child->orient_y,&child->orient_z, &child->pos);

				// and add it at the root object
				child->Insert(this, INSERT_HORIZ);

				child->flags |= OBJECT_DIRTY;

				child = next;
			}
		}
	}
	else
	{
		if(external != EXTERNAL_ROOT)
		{
			obj = (OBJECT*)GoDown();
			if(obj)
				obj->UnGroupObjectsList();
		}
	}

	if(n)
		n->UnGroupObjectsList();
}

void UnGroupObjects()
{
	if(root)
		root->UnGroupObjectsList();
}

/*************
 * DESCRIPTION:   convert all child objects to none-external objects
 * INPUT:         -
 * OUTPUT:        -
 *************/
void OBJECT::ExternToSceneList()
{
	OBJECT *obj, *down;

	obj = this;
	while(obj)
	{
		// make material public
		if(obj->surf)
		{
			obj->surf->flags &= ~(SURF_EXTERNAL | SURF_WRITTEN);
			sciAddMaterial(obj->surf);
		}

		obj->external = EXTERNAL_NONE;

		down = (OBJECT*)obj->GoDown();
		if(down)
			down->ExternToSceneList();

		obj = (OBJECT*)obj->GetNext();
	}
}

void OBJECT::ExternToScene()
{
	OBJECT *down;
	MATRIX m,m1,matrix;

	if(GoDown())
	{
		down = (OBJECT*)GoDown();

		m.SetOTMatrix(&orient_x, &orient_y, &orient_z, &pos);
		m1.SetOTMatrix(&down->orient_x, &down->orient_y, &down->orient_z, &down->pos);
		matrix.MultMat(&m1, &m);
		SetVector(&down->pos, matrix.m[1], matrix.m[2], matrix.m[3]);
		SetVector(&down->orient_x, matrix.m[5], matrix.m[9], matrix.m[13]);
		SetVector(&down->orient_y, matrix.m[6], matrix.m[10], matrix.m[14]);
		SetVector(&down->orient_z, matrix.m[7], matrix.m[11], matrix.m[15]);

		down->IsFirstSelected();
#ifdef __AMIGA__
		sciMaterialInitChange();
#endif
		down->ExternToSceneList();
#ifdef __AMIGA__
		sciMaterialExitChange();
#endif
	}
}

/*************
 * DESCRIPTION:   update the tracking direction
 * INPUT:         pos      object position (if this pointer is NULL then only test if
 *    object is tracking and set return value)
 * OUTPUT:        TRUE if tracking else FALSE
 *************/
BOOL OBJECT::UpdateTracking(VECTOR *pos)
{
	VECTOR v,u,w, trackpos;
	float dist;
	VECTOR p,ox,oy,oz;
	MATRIX m;
	OBJECT *up;

	if(track)
	{
		if(!pos)
			return TRUE;

		flags |= OBJECT_DIRTY;

		track->GetObjectMatrix(&m);
		m.GenerateAxis(&u, &v, &w, &trackpos);
		VecSub(&trackpos,pos,&v);

		// default axis if object pos and track pos are identical
		dist = fabs(VecNormalize(&v));
		if (dist < EPSILON)
		{
			SetVector(&v, 1.f,0.f,0.f);
			SetVector(&u, 0.f,0.f,1.f);
		}
		else
		{
			// prevent division by zero
			if (fabs(v.x) < EPSILON)
				SetVector(&u, 1.f,0.f,0.f);
			else
				SetVector(&u, -v.z/v.x,0.f,1.f);

			VecNormalize(&u);
		}

		VecCross(&v,&u,&w);
		// invert if camera is head over heels
		if (w.y < 0.f)
		{
			VecScale(-1.f, &w, &w);
			VecScale(-1.f, &u, &u);
		}

		// the alignment of child objects needs to be relative to
		// the parent axis
		up = (OBJECT*)this->GoUp();
		if(up)
		{
			up->GetObjectMatrix(&m);
			m.GenerateAxis(&ox, &oy, &oz, &p);
			m.SetOMatrix(&ox,&oy,&oz);
			m.MultMatOrient(&u,&w,&v);
		}

		InvOrient(&u, &w, &v, &orient_x, &orient_y, &orient_z);

		switch(GetType())
		{
			case OBJECT_CAMERA:
				// update focaldistance
				if(((CAMERA*)this)->flags & OBJECT_CAMERA_FOCUSTRACK)
					((CAMERA*)this)->focaldist = dist;
				break;
			case OBJECT_SPOTLIGHT:
			case OBJECT_POINTLIGHT:
				// update falloff-distance
				if(((LIGHT*)this)->flags & OBJECT_LIGHT_TRACKFALLOFF)
					((LIGHT*)this)->falloff = dist;
				break;
		}

		return TRUE;
	}
	return FALSE;
}

/*************
 * DESCRIPTION:   the load routine writes a string to the track pointer, and this
 *    routine searches the object and deletes the string
 * INPUT:         -
 * OUTPUT:        -
 *************/
void OBJECT::TranslateNamesList()
{
	OBJECT *obj,*down,*track;

	obj = this;
	while(obj)
	{
		if(obj->track)
		{
			track = GetObjectByName((char*)obj->track);
			delete (char*)obj->track;
			obj->track = track;
			if(track)
				track->istracked = TRUE;
		}

		down = (OBJECT*)obj->GoDown();
		if(down)
			down->TranslateNamesList();

		obj = (OBJECT*)obj->GetNext();
	}
}

void TranslateNames()
{
	if(root)
		root->TranslateNamesList();
}

/*************
 * DESCRIPTION:   remove all trackings to object
 * INPUT:         track    object to untrack
 * OUTPUT:        -
 *************/
void Untrack(OBJECT *track)
{
	OBJECT *obj;

	obj = root;
	while(obj)
	{
		if(obj->track == track)
			obj->track = NULL;

		obj = obj->Iterate();
	}
}

/*************
 * DESCRIPTION:   duplicate a list of objects
 * INPUT:         -
 * OUTPUT:        TRUE if all went ok else FALSE
 *************/
BOOL OBJECT::DuplicateObjectsList(OBJECT **root, OBJECT *where, int dir)
{
	OBJECT *obj,*down,*dup;

	obj = this;
	while(obj)
	{
		dup = obj->Duplicate();
		if(!dup)
			return FALSE;

		// get absolute axis for objects on root level
/*    if(!where)
			obj->GetAbsObjectAxis(&dup->pos, &dup->orient_x, &dup->orient_y, &dup->orient_z);*/

		if(!where)
			dup->LIST::Append((LIST**)root);
		else
			dup->LIST::Insert((LIST*)where,dir);

		where = dup;

		down = (OBJECT*)obj->GoDown();
		if(down)
		{
			if(!down->DuplicateObjectsList(root, where,INSERT_VERT))
				return FALSE;
		}

		obj = (OBJECT*)obj->GetNext();
		dir = INSERT_HORIZ;
	}
	return TRUE;
}

BOOL DuplicateObjects(OBJECT **root, OBJECT *obj)
{
	if(!obj->DuplicateObjectsList(root, NULL, INSERT_HORIZ))
	{
		// delete duplicated objects on error
		if(*root)
		{
			(*root)->FreeObjectsList();
			*root = NULL;
		}

		return FALSE;
	}
	return TRUE;
}

/*************
 * DESCRIPTION:   get the memory size needed from an object tree
 * INPUT:         -
 * OUTPUT:        size
 *************/
int OBJECT::GetSizeOfTree()
{
	OBJECT *cur, *down;
	int size = 0;

	cur = this;
	while(cur)
	{
		size += cur->GetSize();

		down = (OBJECT*)cur->GoDown();
		if(down)
			size += down->GetSizeOfTree();

		cur = (OBJECT*)cur->GetNext();
	}
	return size;
}

/*************
 * DESCRIPTION:   remove one object and it's childs from list without destroying it
 * INPUT:         -
 * OUTPUT:        -
 *************/
void OBJECT::DeChainObject()
{
	if(firstselected == this)
		NoFirstSelected();
	if(cur_selected == this)
		cur_selected = NULL;

	DeChain((LIST**)&root);
}

/*************
 * DESCRIPTION:   Test is a surface is used by an object. Returns NULL for external surfaces
 * INPUT:         surf
 * OUTPUT:        object which uses surfaces or NULL
 *************/
OBJECT *IsSurfaceUsed(SURFACE *surf)
{
	OBJECT *obj;

	obj = root;
	while(obj)
	{
		if(surf == obj->surf)
			return obj;

		obj = obj->Iterate();
	}
	return NULL;
}

/*************
 * DESCRIPTION:   add a brush object at the end of the list
 * INPUT:         brush    brush to add
 * OUTPUT:        none
 *************/
void OBJECT::AddBrush(BRUSH_OBJECT *brush)
{
	brush->LIST::Append((LIST**)&this->brush);
}

/*************
 * DESCRIPTION:   add a brush object at the top of the list
 * INPUT:         brush    brush to add
 * OUTPUT:        none
 *************/
void OBJECT::AddBrushTop(BRUSH_OBJECT *brush)
{
	if(this->brush)
		this->brush->LIST::Insert((LIST*)brush, INSERT_HORIZ);

	this->brush = brush;
}

/*************
 * DESCRIPTION:   add a texture object at the end of the list
 * INPUT:         texture     texture to add
 * OUTPUT:        none
 *************/
void OBJECT::AddTexture(TEXTURE_OBJECT *texture)
{
	texture->LIST::Append((LIST**)&this->texture);
}

/*************
 * DESCRIPTION:   add a texture object at the top of the list
 * INPUT:         texture     texture to add
 * OUTPUT:        none
 *************/
void OBJECT::AddTextureTop(TEXTURE_OBJECT *texture)
{
	if(this->texture)
		this->texture->LIST::Insert((LIST*)texture, INSERT_HORIZ);

	this->texture = texture;
}

/*************
 * DESCRIPTION:   remove brush definitions
 * INPUT:         -
 * OUTPUT:        -
 *************/
void OBJECT::RemoveBrushes()
{
	BRUSH_OBJECT *brush, *next;

	brush = this->brush;
	while(brush)
	{
		next = (BRUSH_OBJECT*)brush->GetNext();
		delete brush;
		brush = next;
	}
	this->brush = NULL;
}

/*************
 * DESCRIPTION:   remove texture definitions
 * INPUT:         -
 * OUTPUT:        -
 *************/
void OBJECT::RemoveTextures()
{
	TEXTURE_OBJECT *texture, *next;

	texture = this->texture;
	while(texture)
	{
		next = (TEXTURE_OBJECT*)texture->GetNext();
		delete texture;
		texture = next;
	}
	this->texture = NULL;
}

/*************
 * DESCRIPTION:   Update the brush and texture objects of all objects
 *    which are assigned to a specified surface
 * INPUT:         surf     surface
 * OUTPUT:        FALSE if failed else TRUE
 *************/
BOOL UpdateSurfaceAssignment(SURFACE *surf)
{
	OBJECT *obj;
	BRUSH_OBJECT *brushobj, *bnext, *boldlist;
	BRUSH *brush;
	TEXTURE_OBJECT *textureobj, *tnext, *toldlist;
	TEXTURE *texture;
	BOOL found;

	obj = root;
	while(obj)
	{
		if(obj->surf == surf)
		{
			// brushes
			boldlist = obj->brush;
			obj->brush = NULL;

			brush = surf->brush;
			while(brush)
			{
				brushobj = boldlist;
				found = FALSE;
				while(brushobj && !found)
				{
					bnext = (BRUSH_OBJECT*)brushobj->GetNext();

					if(brush == brushobj->brush)
					{
						found = TRUE;
						brushobj->DeChain((LIST**)&boldlist);
						brushobj->Init();
						obj->AddBrush(brushobj);
					}
					brushobj = bnext;
				}
				if(!found)
				{
					// create new brush object
					brushobj = new BRUSH_OBJECT;
					if(!brushobj)
						return FALSE;
					brushobj->brush = brush;
					obj->AddBrush(brushobj);
				}
				brush = (BRUSH*)brush->GetNext();
			}

			// delete unused
			brushobj = boldlist;
			while(brushobj)
			{
				bnext = (BRUSH_OBJECT*)brushobj->GetNext();
				delete brushobj;
				brushobj = bnext;
			}

			// textures
			toldlist = obj->texture;
			obj->texture = NULL;

			texture = surf->texture;
			while(texture)
			{
				textureobj = toldlist;
				found = FALSE;
				while(textureobj && !found)
				{
					tnext = (TEXTURE_OBJECT*)textureobj->GetNext();

					if(texture == textureobj->texture)
					{
						found = TRUE;
						textureobj->DeChain((LIST**)&toldlist);
						textureobj->Init();
						obj->AddTexture(textureobj);
					}
					textureobj = tnext;
				}
				if(!found)
				{
					// create new texture object
					textureobj = new TEXTURE_OBJECT;
					if(!textureobj)
						return FALSE;
					textureobj->texture = texture;
					obj->AddTexture(textureobj);
				}
				texture = (TEXTURE*)texture->GetNext();
			}

			// delete unused
			textureobj = toldlist;
			while(textureobj)
			{
				tnext = (TEXTURE_OBJECT*)textureobj->GetNext();
				delete textureobj;
				textureobj = tnext;
			}
		}

		obj = obj->Iterate();
	}
	return TRUE;
}

/*************
 * DESCRIPTION:   Increase the open counter of all selected
 *    objects
 * INPUT:         win      requester
 * OUTPUT:        -
 *************/
#ifdef __AMIGA__
void OBJECT::IncOpenCounterList(Object *win)
{
	OBJECT *obj, *down;
	BRUSH_OBJECT *brush;
	TEXTURE_OBJECT *texture;

	obj = this;
	while(obj)
	{
		if(obj->selected)
		{
#ifdef __AMIGA__
			obj->win = win;
#endif
			obj->opencnt++;
		}
		else
		{
			if(obj->brush)
			{
				brush = obj->brush;
				while(brush)
				{
					if(brush->selected)
					{
#ifdef __AMIGA__
						brush->win = win;
#endif
						brush->opencnt++;
					}
					brush = (BRUSH_OBJECT*)brush->GetNext();
				}
			}

			if(obj->texture)
			{
				texture = obj->texture;
				while(texture)
				{
					if(texture->selected)
					{
#ifdef __AMIGA__
						texture->win = win;
#endif
						texture->opencnt++;
					}
					texture = (TEXTURE_OBJECT*)texture->GetNext();
				}
			}

			down = (OBJECT*)obj->GoDown();
			if(down)
				down->IncOpenCounterList(win);
		}

		obj = (OBJECT*)obj->GetNext();
	}
}
#endif

#ifdef __AMIGA__
void IncOpenCounter(Object *win)
{
	if(root)
		root->IncOpenCounterList(win);
}
#endif

/*************
 * DESCRIPTION:   remove the dirty flag from all objects
 * INPUT:         -
 * OUTPUT:        -
 *************/
void RemoveDirtyFlag()
{
	OBJECT *obj;

	obj = root;
	while(obj)
	{
		obj->flags &= ~OBJECT_DIRTY;
		obj = obj->Iterate();
	}
}

#ifdef __OPENGL__
/*************
 * DESCRIPTION:   setup the lights for the OpenGL view
 * INPUT:         -
 * OUTPUT:        -
 *************/
void SetupOpenGLLights(DISPLAY *display)
{
	MATRIX_STACK stack;
	MATRIX m, modify;
	OBJECT *obj;
	GLint maxlights, lights, i;
	GLfloat glpos[4];
	GLfloat color[4];
	GLfloat dir[3] = { -1.f, -1.f, -1.f};
	LIGHT *light;

	PPC_STUB(glGetIntegerv)(GL_MAX_LIGHTS, &maxlights);
	lights = 0;

	modify.SetSOTMatrix(&display->size, &display->orient_x,&display->orient_y,&display->orient_z, &display->pos);

	obj = root;
	while(obj && (lights < maxlights))
	{
		m.SetOTMatrix(&obj->orient_x, &obj->orient_y, &obj->orient_z, &obj->pos);
		stack.Push(&m);

		if((obj->GetType() == OBJECT_POINTLIGHT) || (obj->GetType() == OBJECT_SPOTLIGHT))
		{
			light = (LIGHT*)obj;
			PPC_STUB(glEnable)((GLenum)(GL_LIGHT0+lights));
			PPC_STUB(glPushMatrix)();
			if(light->selected && (display->editmode & EDIT_ACTIVE) && (display->editmode & EDIT_OBJECT))
			{
				stack.Push(&modify);
				OGLLoadMatrix(stack.GetMatrix());
				glpos[0] = light->pos.x + display->pos.x;
				glpos[1] = light->pos.y + display->pos.y;
				glpos[2] = light->pos.z + display->pos.z;
				glpos[3] = 1.f;   // currently only point lights are supported
				PPC_STUB(glLightfv)((GLenum)(GL_LIGHT0+lights), GL_POSITION, glpos);
				stack.Pop(&modify);
			}
			else
			{
				OGLLoadMatrix(stack.GetMatrix());
				glpos[0] = light->pos.x;
				glpos[1] = light->pos.y;
				glpos[2] = light->pos.z;
				glpos[3] = 1.f;   // currently only point lights are supported
				PPC_STUB(glLightfv)((GLenum)(GL_LIGHT0+lights), GL_POSITION, glpos);
			}
			color[0] = light->color.r;
			color[1] = light->color.g;
			color[2] = light->color.b;
			color[3] = 0.f;
			PPC_STUB(glLightfv)((GLenum)(GL_LIGHT0+lights), GL_DIFFUSE, color);
			color[0] = 1.f;
			color[1] = 1.f;
			color[2] = 1.f;
			color[3] = 0.f;
			PPC_STUB(glLightfv)((GLenum)(GL_LIGHT0+lights), GL_SPECULAR, color);
			PPC_STUB(glPopMatrix)();
/*         glLightfv((GLenum)(GL_LIGHT0+lights), GL_SPOT_DIRECTION, dir);
			glLightf((GLenum)(GL_LIGHT0+lights), GL_SPOT_CUTOFF, 50.f);
			glLightf((GLenum)(GL_LIGHT0+lights), GL_SPOT_EXPONENT, 90.f);
			glLightf((GLenum)(GL_LIGHT0+lights), GL_CONSTANT_ATTENUATION, 1.f);
			glLightf((GLenum)(GL_LIGHT0+lights), GL_LINEAR_ATTENUATION, 0.f);
			glLightf((GLenum)(GL_LIGHT0+lights), GL_QUADRATIC_ATTENUATION, 0.f);*/
			lights++;
		}
		obj = obj->Iterate(&stack);
	}

	// disable unused lights
	for(i=lights; i<maxlights; i++)
		PPC_STUB(glDisable)((GLenum)(GL_LIGHT0+i));

	if(!lights)
	{
		// add a default light, if no light is defined
		PPC_STUB(glEnable)(GL_LIGHT0);
		glpos[0] = 100.f;
		glpos[1] = 100.f;
		glpos[2] = -100.f;
		glpos[3] = 1.f;   // currently only point lights are supported
		PPC_STUB(glLightfv)(GL_LIGHT0, GL_POSITION, glpos);
		color[0] = 1.f;
		color[1] = 1.f;
		color[2] = 1.f;
		color[3] = 0.f;
		PPC_STUB(glLightfv)(GL_LIGHT0, GL_DIFFUSE, color);
		color[0] = 1.f;
		color[1] = 1.f;
		color[2] = 1.f;
		color[3] = 0.f;
		PPC_STUB(glLightfv)(GL_LIGHT0, GL_SPECULAR, color);
/*      glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, dir);
		glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 50.f);
		glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 90.f);
		glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.f);
		glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.f);
		glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.f);*/
	}
}
#endif // __OPENGL__

/*************
 * DESCRIPTION:   Calculates the overall scene bounding box
 * INPUT:         bboxmin, bboxmax
 * OUTPUT:        none
 *************/
static void ExtendSceneBBox(VECTOR *v, VECTOR *bboxmin, VECTOR *bboxmax)
{
	if(v->x < bboxmin->x)
		bboxmin->x = v->x;
	if(v->x > bboxmax->x)
		bboxmax->x = v->x;

	if(v->y < bboxmin->y)
		bboxmin->y = v->y;
	if(v->y > bboxmax->y)
		bboxmax->y = v->y;

	if(v->z < bboxmin->z)
		bboxmin->z = v->z;
	if(v->z > bboxmax->z)
		bboxmax->z = v->z;
}

void CalcSceneBBox(DISPLAY *display, VECTOR *bboxmin, VECTOR *bboxmax)
{
	MATRIX_STACK stack;
	OBJECT *obj = root;
	MATRIX m, modify;
	VECTOR v;

	SetVector(bboxmin, INFINITY, INFINITY, INFINITY);
	SetVector(bboxmax, -INFINITY, -INFINITY, -INFINITY);

	modify.SetSOTMatrix(&display->size, &display->orient_x,&display->orient_y,&display->orient_z, &display->pos);

	while(obj)
	{
		m.SetOTMatrix(&obj->orient_x, &obj->orient_y, &obj->orient_z, &obj->pos);
		stack.Push(&m);

		if(obj->selected && (display->editmode & EDIT_ACTIVE) && (display->editmode & EDIT_OBJECT))
			stack.Push(&modify);

		v = obj->bboxmin;
		stack.MultVectStack(&v);
		ExtendSceneBBox(&v, bboxmin, bboxmax);

		SetVector(&v, obj->bboxmin.x, obj->bboxmin.y, obj->bboxmax.z);
		stack.MultVectStack(&v);
		ExtendSceneBBox(&v, bboxmin, bboxmax);

		SetVector(&v, obj->bboxmin.x, obj->bboxmax.y, obj->bboxmin.z);
		stack.MultVectStack(&v);
		ExtendSceneBBox(&v, bboxmin, bboxmax);

		SetVector(&v, obj->bboxmin.x, obj->bboxmax.y, obj->bboxmax.z);
		stack.MultVectStack(&v);
		ExtendSceneBBox(&v, bboxmin, bboxmax);

		SetVector(&v, obj->bboxmax.x, obj->bboxmin.y, obj->bboxmin.z);
		stack.MultVectStack(&v);
		ExtendSceneBBox(&v, bboxmin, bboxmax);

		SetVector(&v, obj->bboxmax.x, obj->bboxmin.y, obj->bboxmax.z);
		stack.MultVectStack(&v);
		ExtendSceneBBox(&v, bboxmin, bboxmax);

		SetVector(&v, obj->bboxmax.x, obj->bboxmax.y, obj->bboxmin.z);
		stack.MultVectStack(&v);
		ExtendSceneBBox(&v, bboxmin, bboxmax);

		v = obj->bboxmin;
		stack.MultVectStack(&v);
		ExtendSceneBBox(&v, bboxmin, bboxmax);

		if(obj->selected && (display->editmode & EDIT_ACTIVE) && (display->editmode & EDIT_OBJECT))
			stack.Pop();

		obj = obj->Iterate(&stack);
	}
}


